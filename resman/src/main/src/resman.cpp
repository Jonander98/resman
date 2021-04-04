/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resman.h"

using WorkScheduling::work_group;

resman::~resman()
{
  m_work_group.stop_execution();
  unload_all();
  //Clean all the registered resource containers
  for (auto pair : m_resources)
    delete pair.second;
  m_resources.clear();
}

resman::resman(config cf)
  : m_config(cf), m_work_group(work_group::config{cf.max_threads, cf.min_resources_to_fork})
{}


void resman::unload_all()
{
  for (auto pair : m_resources)
  {
    //Get the current container as a container of the parent resource class
    auto ct = reinterpret_cast<resource_container<resource>*>(pair.second);
    for (auto & in_pair : *ct)
    {
      //Call the unload trough the virtual table
      in_pair.second->internal_unload();
      delete in_pair.second.get();
    }
    ct->clear();
  }
}

void resman::set_config(config c)
{
  m_config = c;
}

void resman::set_log(const message_log & log)
 {
   m_log = log;
 }

 const message_log & resman::get_log() const
 {
   return m_log;
 }

 const message_log& resman::get_resource_manager_status()
 {
   u32 resource_count = 0;
   u32 never_used_resource_count = 0;
   u32 unused_resource_count = 0;
   for (auto pair : m_resources)
   {
     //Get the current container as a container of the parent resource class
     auto ct = reinterpret_cast<resource_container<resource>*>(pair.second);
     for (auto& in_pair : *ct)
     {
       if (!in_pair.second->m_any_use)
         never_used_resource_count++;
       if (*in_pair.second.m_ref_count == 1)
         unused_resource_count++;
       resource_count++;
     }
   }
   m_log.info("Resource manager status");

   m_log.info(
     "Loaded resource count: " + std::to_string(resource_count) + '\n' +
     "Never used resource count: " + std::to_string(never_used_resource_count) + '\n' +
     "Currently unused resource count: " + std::to_string(unused_resource_count));

   return m_log;
 }


 void resman::save_to_file(const filepath & fp, bool only_ever_used) const
 {
   std::ofstream file;
   file.open(fp.get_fullpath());
   if (!file.is_open())
     return;

   std::stringstream buff;
   auto push_resource = [&buff](RTTI::type type_id, const filepath & path)
   {
     buff << type_id.get_hash() << ' ' << path.get_fullpath() << '\n';
   };

   for (auto pair : m_resources)
   {
     //Get the current container as a container of the parent resource class
     auto ct = reinterpret_cast<resource_container<resource>*>(pair.second);
     for (auto& in_pair : *ct)
     {
       if (only_ever_used)
       {
         if (in_pair.second->m_any_use)
           push_resource(pair.first, in_pair.second->m_path);
       }
       else
         push_resource(pair.first, in_pair.second->m_path);
     }
   }
   file << buff.str();
 }

 void resman::from_file(const filepath& fp)
 {
   for_each_resource_in_file(fp, [this](const filepath& fp, size_t id)
   {
     if (is_registered(id))
     {
       XASSERTMSG(false, "From file not supported");
       //reinterpret_cast<resource_container<resource> *>(m_resources.find(id)->second)->emplace(id, fp);
     }
   });
 }

 bool resman::is_registered(RTTI::type typeId)
 {
   return m_resources.find(typeId) != m_resources.end();
 }

 void resman::for_each_resource_in_file(const filepath& fp, std::function<void(const filepath&, size_t)> action) const
 {
   std::ifstream file;
   file.open(fp.get_fullpath());

   if (!file.is_open())
     return;

   std::array<char, 200> buff;
   std::array<char, 1> separators{ ' ' };

   while (!file.eof())
   {
     buff.fill(0);
     file.getline(buff.data(), buff.size());
     //Parse line
     auto it = std::find_first_of(buff.begin(), buff.end(), separators.begin(), separators.end());
     if (it == buff.end())
     {
       m_log.error("Loading from file: File was modified or not well saved");
       continue;
     }

     size_t id = std::stoull(buff.data());
     filepath resource_file{ std::string(++it, buff.end()) };

     action(resource_file, id);
   }
   file.close();
 }

