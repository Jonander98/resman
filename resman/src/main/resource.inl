/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
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
  : m_ptr(rhs.m_ptr), m_ref_count(rhs.m_ref_count)
{
  //One reference more
  if(m_ptr)
    (*m_ref_count)++;
}

template<typename resource_type>
inline resource_ptr<resource_type>::resource_ptr()
  : resource_ptr(nullptr)
{}

template<typename resource_type>
inline resource_ptr<resource_type>::~resource_ptr()
{
  remove_ownership();
}

template<typename resource_type>
inline resource_ptr<resource_type>::resource_ptr(resource_type * ptr)
  : m_ptr(ptr)
{
  if (ptr)
  {//Start counting references
    m_ref_count = new i32(0);
    (*m_ref_count)++;
  }
}


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
  remove_ownership();
  //Copy both the pointer and the reference counter
  m_ptr = rhs.m_ptr;
  m_ref_count = rhs.m_ref_count;
  if (m_ptr)
    (*m_ref_count)++;
  return *this;
}

template<typename resource_type>
inline resource_ptr<resource_type>::operator resource_type*()
{
  return m_ptr;
}

template<typename resource_type>
inline i32 resource_ptr<resource_type>::get_reference_count() const
{
  //We are removing one for the user as the resource manager is keeping one
  return m_ptr ? *m_ref_count - 1 : 0;
}

template<typename resource_type>
inline void resource_ptr<resource_type>::remove_ownership()
{
  if (m_ptr == nullptr)
    return;
  (*m_ref_count)--;
  if (*m_ref_count <= 0)
  {
    delete m_ref_count;
    //The resource memory is not owned by the pointer
    //delete m_ptr;
  }
}

template<typename resource_type>
inline bool resource_ptr<resource_type>::is_valid()const
{
  return m_ptr != nullptr;
}
#pragma endregion