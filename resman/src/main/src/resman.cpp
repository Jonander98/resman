/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resman.h"

using WorkScheduling::WorkGroup;

Resman::~Resman()
{
  m_workGroup.StopExecution();
  UnloadAll();
  //Clean all the registered resource containers
  for (auto pair : m_resources)
    delete pair.second;
  m_resources.clear();
}

Resman::Resman(Config cf)
  : m_config(cf), m_workGroup(WorkGroup::Config{cf.maxThreads, cf.minResourcesToFork})
{}


void Resman::UnloadAll()
{
  for (auto pair : m_resources)
  {
    //Get the current container as a container of the parent resource class
    auto ct = reinterpret_cast<resource_container<AResource>*>(pair.second);
    for (auto & in_pair : *ct)
    {
      //Call the unload trough the virtual table
      in_pair.second->InternalUnload();
      delete in_pair.second.Get();
    }
    ct->clear();
  }
}

void Resman::SetConfig(Config c)
{
  m_config = c;
}

void Resman::SetLog(const MessageLog & log)
 {
   m_log = log;
 }

 const MessageLog & Resman::GetLog() const
 {
   return m_log;
 }

 const MessageLog& Resman::GetResourceManagerStatus()
 {
   u32 resource_count = 0;
   u32 never_used_resource_count = 0;
   u32 unused_resource_count = 0;
   for (auto pair : m_resources)
   {
     //Get the current container as a container of the parent resource class
     auto ct = reinterpret_cast<resource_container<AResource>*>(pair.second);
     for (auto& in_pair : *ct)
     {
       if (!in_pair.second->m_anyUse)
         never_used_resource_count++;
       if (*in_pair.second.m_refCount == 1)
         unused_resource_count++;
       resource_count++;
     }
   }
   m_log.Info("Resource manager status");

   m_log.Info(
     "Loaded resource count: " + std::to_string(resource_count) + '\n' +
     "Never used resource count: " + std::to_string(never_used_resource_count) + '\n' +
     "Currently unused resource count: " + std::to_string(unused_resource_count));

   return m_log;
 }


 void Resman::SaveToFile(const Filepath & fp, bool only_ever_used) const
 {
   std::ofstream file;
   file.open(fp.GetFullPath());
   if (!file.is_open())
     return;

   std::stringstream buff;
   auto push_resource = [&buff](RTTI::Type type_id, const Filepath & path)
   {
     buff << type_id.GetHash() << ' ' << path.GetFullPath() << '\n';
   };

   for (auto pair : m_resources)
   {
     //Get the current container as a container of the parent resource class
     auto ct = reinterpret_cast<resource_container<AResource>*>(pair.second);
     for (auto& in_pair : *ct)
     {
       if (only_ever_used)
       {
         if (in_pair.second->m_anyUse)
           push_resource(pair.first, in_pair.second->m_path);
       }
       else
         push_resource(pair.first, in_pair.second->m_path);
     }
   }
   file << buff.str();
 }

 void Resman::FromFile(const Filepath& fp)
 {
   ForEachResourceInFile(fp, [this](const Filepath& fp, size_t id)
   {
     if (IsRegistered(id))
     {
       (void)fp;
       XASSERTMSG(false, "From file not supported");
       //reinterpret_cast<resource_container<resource> *>(m_resources.find(id)->second)->emplace(id, fp);
     }
   });
 }

 bool Resman::IsRegistered(RTTI::Type typeId)
 {
   return m_resources.find(typeId) != m_resources.end();
 }

 void Resman::ForEachResourceInFile(const Filepath& fp, std::function<void(const Filepath&, size_t)> action) const
 {
   std::ifstream file;
   file.open(fp.GetFullPath());

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
       m_log.Error("Loading from file: File was modified or not well saved");
       continue;
     }

     size_t id = std::stoull(buff.data());
     Filepath resource_file{ std::string(++it, buff.end()) };

     action(resource_file, id);
   }
   file.close();
 }

