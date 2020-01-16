

#pragma region resource
template<typename resource_type>
inline resource::id_type resource::compute_id(const str_t & st)
{
  //NOTE: this hash function does not guarantee uniqueness, but it is incredibly rare to have collisions
  return resource_hash<resource_type>{}(st);
}
#pragma endregion
#pragma region resource_ptr
template<typename resource_type>
inline resource_ptr<resource_type>::resource_ptr(const resource_ptr & rhs)
  : m_ptr(rhs.m_ptr)
{}


template<typename resource_type>
inline resource_type * resource_ptr<resource_type>::get()
{
  return m_ptr;
}

template<typename resource_type>
inline resource_type * resource_ptr<resource_type>::operator->() const
{
  return m_ptr;
}

template<typename resource_type>
inline resource_ptr<resource_type> & resource_ptr<resource_type>::operator=(const resource_ptr & rhs)
{
  m_ptr = rhs.m_ptr;
  return *this;
}

template<typename resource_type>
inline resource_ptr<resource_type>::operator resource_type*()
{
  return m_ptr;
}

template<typename resource_type>
inline bool resource_ptr<resource_type>::is_valid()
{
  return m_ptr != nullptr;
}
#pragma endregion