/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

template<typename resource_type>
inline resource_ptr<resource_type> resman::get(const str_t & st)
{
  check_resource_type<resource_type>();

  resource_ptr<resource_type> ret_ptr;//null

  size_t id = typeid(resource_type).hash_code();
  auto it = m_resources.find(id);
  auto ct = get_resource_container<resource_type>();
  if (ct == nullptr)
  {
    //The resource is not registered
    XBREAK();
    return ret_ptr;
  }

  auto it2 = ct->find(resource::compute_id<resource_type>(st));
  if (it2 == ct->end())
  {
    //Resource not found
    return ret_ptr;
  }
  //Found
  ret_ptr.m_ptr = &it2->second;
  return ret_ptr;
}

template<typename resource_type>
inline std::vector<resource_ptr<resource_type>> resman::get_all()
{
  check_resource_type<resource_type>();

  auto cont_ptr = get_resource_container<resource_type>();
  if (cont_ptr == nullptr)
  {//Resource not registered
    XBREAK();
    return {};
  }
  //Create the container
  std::vector<resource_ptr<resource_type>> ret;
  ret.reserve(cont_ptr->size());
  //Fill it with all the resources
  for (auto & pair : *cont_ptr)
    ret.push_back(&pair.second);

  return ret;
}

template<typename resource_type>
inline void resman::load(const file_path & fp)
{
  check_resource_type<resource_type>();
  if (get<resource_type>(fp.get_full_name()) != nullptr)
  {//Resource already loaded
    XBREAK();
    return;
  }
  auto * cont = get_resource_container<resource_type>();
  if (cont == nullptr)
  {//Resource not registered
    XBREAK();
    return;
  }
  //Create an storage for the resource
  resource::id_type res_id = resource::compute_id<resource_type>(fp.get_full_name());
  auto & result = cont->try_emplace(res_id, (resource_type())).first->second;
  //auto & result = (*cont)[res_id] = resource_type();
  //Set the id of the resource
  result.m_id = res_id;
  //Start the load (syncronous for now)
  result.internal_load(fp);
}

template<typename resource_type>
inline void resman::register_resource()
{
  check_resource_type<resource_type>();
  size_t id = typeid(resource_type).hash_code();
  if (m_resources.find(id) != m_resources.end())
  {
    //Resource already registered
    XBREAK();
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
inline void resman::check_resource_type()
{
  static_assert(std::is_base_of<resource, resource_type>::value,
    "Every resource must derive from the resource class");
  static_assert(std::is_default_constructible<resource_type>::value,
    "Every resource must have a default constructor");
}
