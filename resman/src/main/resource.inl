/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma region resource
template<typename resource_type>
inline AResource::IdType AResource::ComputeId(const str_t & st)
{
  //NOTE: this hash function does not guarantee uniqueness, but it is incredibly rare to have collisions
  return ResourceHash<resource_type>{}(st);
}
#pragma endregion
#pragma region resource_ptr
template<typename resource_type>
inline ResourcePtr<resource_type>::ResourcePtr(const ResourcePtr & rhs)
  : m_ptr(rhs.m_ptr), m_refCount(rhs.m_refCount)
{
  //One reference more
  if (m_ptr)
  {
    (*m_refCount)++;
  }
}

template<typename resource_type>
inline ResourcePtr<resource_type>::ResourcePtr()
  : ResourcePtr(nullptr)
{}

template<typename resource_type>
inline ResourcePtr<resource_type>::~ResourcePtr()
{
  RemoveOwnership();
}

template<typename resource_type>
inline ResourcePtr<resource_type>::ResourcePtr(resource_type * ptr)
  : m_ptr(ptr)
{
  if (ptr)
  {//Start counting references
    m_refCount = new std::atomic<i32>(0);
    (*m_refCount)++;
  }
}


template<typename resource_type>
inline resource_type * ResourcePtr<resource_type>::Get()
{
  return m_ptr;
}

template<typename resource_type>
inline resource_type * ResourcePtr<resource_type>::operator->() const
{
  return m_ptr;
}

template<typename resource_type>
inline ResourcePtr<resource_type> & ResourcePtr<resource_type>::operator=(const ResourcePtr & rhs)
{
  RemoveOwnership();
  //Copy both the pointer and the reference counter
  m_ptr = rhs.m_ptr;
  m_refCount = rhs.m_refCount;
  if (m_ptr)
  {
    (*m_refCount)++;
  }
  return *this;
}

template<typename resource_type>
inline ResourcePtr<resource_type>::operator resource_type*()
{
  return m_ptr;
}

template<typename resource_type>
inline i32 ResourcePtr<resource_type>::GetReferenceCount() const
{
  //We are removing one for the user as the resource manager is keeping one
  return m_ptr ? *m_refCount - 1 : 0;
}

template<typename resource_type>
inline void ResourcePtr<resource_type>::RemoveOwnership()
{
  if (m_ptr == nullptr)
    return;
  //Just remove a reference
  (*m_refCount)--;
  if (*m_refCount <= 0)
  {
    delete m_refCount;
    //The resource memory is not owned by the pointer
    //delete m_ptr;
  }
}

template<typename resource_type>
inline bool ResourcePtr<resource_type>::IsValid()const
{
  return m_ptr != nullptr;
}
#pragma endregion