/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

template<typename resource_type>
inline resource_ptr<resource_type> resman::get(const str_t & st)
{
  check_resource_type<resource_type>();

  resource_ptr<resource_type> ret_ptr;//null

  auto ct = get_resource_container<resource_type>();
  if (ct == nullptr)
  {
    //The resource is not registered
    m_log.warning("get: The resource is not registered");
    return ret_ptr;
  }

  auto it = ct->find(resource::compute_id<resource_type>(st));
  if (it == ct->end())
  {
    //Resource not found
    //m_log.warning("get: No load function has been called with the given parameters");
    return ret_ptr;
  }
  //Found
  return it->second;
}

template<typename resource_type>
inline std::vector<resource_ptr<resource_type>> resman::get_all_t()
{
  check_resource_type<resource_type>();

  auto cont_ptr = get_resource_container<resource_type>();
  if (cont_ptr == nullptr)
  {//Resource not registered
    m_log.warning("get_all: The resource is not registered");
    return {};
  }
  //Create the container
  std::vector<resource_ptr<resource_type>> ret;
  ret.reserve(cont_ptr->size());
  //Fill it with all the resources
  for (auto & pair : *cont_ptr)
    ret.push_back(pair.second);

  return ret;
}

template<typename resource_type>
inline void resman::load(const filepath & fp)
{
  internal_load<resource_type>(fp, false);
}

template<typename resource_type>
inline void resman::load_async(const filepath & fp)
{
  internal_load<resource_type>(fp, true);
}

template<typename resource_type>
inline void resman::unload(const str_t & st)
{
  check_resource_type<resource_type>();
  auto ct = get_resource_container<resource_type>();
  if (ct == nullptr)
  {
    m_log.info("unload: The resource is not registered");
    return;
  }
  auto it = ct->find(resource::compute_id<resource_type>(st));
  if (it != ct->end())
  {
    it->second->internal_unload();
    if(it->second.get_reference_count() > 0)
      m_log.info("unload: The resource was still in use");
    //Deallocate the memory
    delete it->second.get();
    ct->erase(it);
  }
}

template<typename resource_type>
inline void resman::unload_all_t()
{
  check_resource_type<resource_type>();

  auto ct = get_resource_container<resource_type>();
  if (ct == nullptr)
  {
    m_log.info("unload_all_t: The resource is not registered");
    return;
  }
  for (auto & pair : *ct)
  {
    pair.second->internal_unload();
    delete pair.second.get();
  }
  ct->clear();
}

template<typename resource_type>
inline void resman::register_resource()
{
  check_resource_type<resource_type>();
  size_t id = typeid(resource_type).hash_code();
  if (m_resources.find(id) != m_resources.end())
  {
    //Resource already registered
    m_log.info("register_resource: The resource is already registered");
    return;
  }
  m_resources[id] = new resource_container<resource_type>;
}
template <typename resource_type, typename resource_type2, typename ...args>
inline void resman::register_resource()
{
  register_resource<resource_type>();
  register_resource<resource_type2, args...>();
}
template<typename resource_type>
inline bool resman::is_registered()
{
  check_resource_type<resource_type>();
  return get_resource_container<resource_type>() != nullptr;
}

template<typename resource_type>
inline resman::resource_container<resource_type>* resman::get_resource_container()
{
  size_t id = typeid(resource_type).hash_code();
  auto it = m_resources.find(id);
  return (it == m_resources.end()) ? nullptr : reinterpret_cast<resource_container<resource_type>*>(it->second);

}

template<typename resource_type>
inline constexpr void resman::check_resource_type()
{
  static_assert(std::is_base_of<resource, resource_type>::value,
    "Every resource must derive from the resource class");
  static_assert(std::is_default_constructible<resource_type>::value,
    "Every resource must have a default constructor");
}

template<typename resource_type>
inline void resman::internal_load(const filepath & fp, bool is_async)
{
  check_resource_type<resource_type>();
  auto * cont = get_resource_container<resource_type>();
  if (cont == nullptr)
  {//Resource not registered
    m_log.warning("load: The resource is not registered");
    return;
  }
  if (get<resource_type>(fp.get_full_name()) != nullptr)
  {//Resource already loaded
    m_log.info("load: The resource is already loaded");
    return;
  }

  //Create an storage for the resource
  resource::id_type res_id = resource::compute_id<resource_type>(fp.get_full_name());
  auto & res_ptr = cont->try_emplace(res_id, resource_ptr<resource_type>()).first->second;

  //Allocate the resource
  res_ptr = resource_ptr<resource_type>(new resource_type);

  //Set the id of the resource
  res_ptr->m_id = res_id;
  //Start the load
  if (is_async)
  {
    worker::task t;
    t.who = res_ptr.get();
    t.what = &resource_type::internal_load;
    t.how = fp;
    worker * w = find_best_worker();
    if (w == nullptr)
      return;
    w->add_task(t);
  }
  else
  {
    res_ptr->internal_load(fp);
  }
}
