
/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

template<typename resource_type>
inline resource_ptr<resource_type> resman::get(const str_t & st)
{
  static_assert(std::is_base_of<resource, resource_type>::value,
    "Every resource must derive from the resource class");

  resource_ptr<resource_type> ret_ptr;//null

  size_t id = typeid(resource_type).hash_code();
  auto it = m_resources.find(id);
  if (it == m_resources.end())
  {
    //The resource is not registered
    XBREAK();
    return ret_ptr;
  }

  auto & ct = *reinterpret_cast<resource_container<resource_type> *>(it->second);
  auto it2 = ct.find(resource::compute_id<resource_type>(st));
  if (it2 == ct.end())
  {
    //Resource not found
    return ret_ptr;
  }
  //Found
  ret_ptr.m_ptr = &it2->second;
  return ret_ptr;
}

template<typename resource_type>
inline void resman::register_resource()
{
  static_assert(std::is_base_of<resource, resource_type>::value,
    "Every resource must derive from the resource class");
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
  register_resource<resource_type2, args>();

}
