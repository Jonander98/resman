/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

template<typename resource_type>
inline ResourcePtr<resource_type> Resman::Get(const str_t & st)
{
  ResourcePtr<resource_type> retPtr;//null

  auto ct = GetResourceContainer<resource_type>();
  if (ct == nullptr)
  {
    //The resource is not registered
    m_log.Warning("get: The resource is not registered");
    return retPtr;
  }
  AResource::IdType id = AResource::ComputeId<resource_type>(st);
  auto it = ct->find(id);
  if (it == ct->end())
  {
    //Resource not found
    //m_log.warning("get: No load function has been called with the given parameters");
    return retPtr;
  }
  //Found
  it->second->m_anyUse = true;
  return it->second;
}

template<typename resource_type>
inline std::vector<ResourcePtr<resource_type>> Resman::GetAllOfType()
{
  auto contPtr = GetResourceContainer<resource_type>();
  if (contPtr == nullptr)
  {//Resource not registered
    m_log.Warning("get_all: The resource is not registered");
    return {};
  }
  //Create the container
  std::vector<ResourcePtr<resource_type>> ret;
  ret.reserve(contPtr->size());
  //Fill it with all the resources
  for (auto& pair : *contPtr)
  {
    pair.second->m_anyUse = true;
    ret.push_back(pair.second);
  }

  return ret;
}


template <typename resource_type, typename resource_type2, typename ...args>
void Resman::LoadIfSameType(RTTI::Type id, const Filepath& fp)
{
  if (RTTI::Type(typeid(resource_type)) == id)
  {
    //Add it
    Load<resource_type>(fp);
  }
  else
  {
    LoadIfSameType<resource_type2, args...>(id, fp);
  }

}
template <typename resource_type>
void Resman::LoadIfSameType(RTTI::Type id, const Filepath& fp)
{
  if (RTTI::Type(typeid(resource_type)) == id)
  {
    //Add it
    Load<resource_type>(fp);
  }
}

template<typename ...resource_types>
void Resman::FromFileRestrictedTypes(const Filepath& listPath)
{
  static_assert(sizeof...(resource_types) != 0, "Cant load from file with restricted types if no types were given");

  ForEachResourceInFile(listPath, [this](const Filepath & fp, size_t id)
  {
    LoadIfSameType<resource_types...>(id, fp);
  });
}


template<typename resource_type>
inline void Resman::Load(const Filepath & fp)
{
  InternalLoad<resource_type>(fp, false);
}

template<typename resource_type>
inline void Resman::LoadAsync(const Filepath & fp)
{
  InternalLoad<resource_type>(fp, true);
}

template<typename resource_type>
inline void Resman::Unload(const str_t & st)
{
  auto ct = GetResourceContainer<resource_type>();
  if (ct == nullptr)
  {
    m_log.Info("unload: The resource is not registered");
    return;
  }
  auto it = ct->find(AResource::ComputeId<resource_type>(st));
  if (it != ct->end())
  {
    //Tell the resource to unload. Always syncronous
    it->second->InternalUnload();
    if (it->second.GetReferenceCount() > 0)
    {
      m_log.Info("unload: The resource was still in use");
    }
    //Deallocate the memory
    delete it->second.Get();
    ct->erase(it);
  }
}

template<typename resource_type>
inline void Resman::UnloadAllOfType()
{
  auto ct = GetResourceContainer<resource_type>();
  if (ct == nullptr)
  {
    m_log.Info("unload_all_t: The resource is not registered");
    return;
  }
  for (auto & pair : *ct)
  {
    //Unload each of the resources of the given type
    pair.second->InternalUnload();
    if (pair.second.GetReferenceCount() > 0)
    {
      m_log.Info("unload: The resource was still in use");
    }
    delete pair.second.Get();
  }
  ct->clear();
}

template<typename resource_type>
inline void Resman::RegisterResource()
{
  CheckResourceType<resource_type>();
  RTTI::Type id = RTTI::Type(typeid(resource_type));
  if (m_resources.find(id) != m_resources.end())
  {
    //Resource already registered
    m_log.Info("register_resource: The resource is already registered");
    return;
  }
  m_resources[id] = new resource_container<resource_type>;
}
template <typename resource_type, typename resource_type2, typename ...args>
inline void Resman::RegisterResource()
{
  //Register recursively
  RegisterResource<resource_type>();
  RegisterResource<resource_type2, args...>();
}
template<typename resource_type>
inline bool Resman::IsRegistered()
{
  return GetResourceContainer<resource_type>() != nullptr;
}

template<typename resource_type>
inline Resman::resource_container<resource_type>* Resman::GetResourceContainer()
{
  std::type_index id = typeid(resource_type);
  auto it = m_resources.find(id);
  if (it == m_resources.end())
    return nullptr;
  //Found
  return reinterpret_cast<resource_container<resource_type>*>(it->second);
}

template<typename resource_type>
inline constexpr void Resman::CheckResourceType()
{
  static_assert(std::is_base_of<AResource, resource_type>::value,
    "Every resource must derive from the resource class");
  static_assert(std::is_default_constructible<resource_type>::value,
    "Every resource must have a default constructor");
}

template<typename resource_type>
inline void Resman::InternalLoad(const Filepath & fp, bool is_async)
{
  auto * cont = GetResourceContainer<resource_type>();
  if (cont == nullptr)
  {//Resource not registered
    m_log.Warning("load: The resource is not registered");
    return;
  }
  if (Get<resource_type>(fp.GetFullName()) != nullptr)
  {//Resource already loaded
    m_log.Info("load: The resource is already loaded");
    return;
  }

  //Create an storage for the resource
  AResource::IdType resId = AResource::ComputeId<resource_type>(fp.GetFullName());
  ResourcePtr<resource_type>& resPtr = cont->emplace(resId, ResourcePtr<resource_type>()).first->second;

  //Allocate the resource
  resPtr = ResourcePtr<resource_type>(new resource_type);

  //Set the id of the resource
  resPtr->m_id = resId;
  //Start the load
  if (is_async)
  {
    //Create the task
    WorkScheduling::Task t = std::bind(&resource_type::InternalLoad, resPtr.Get(), fp);

    m_workGroup.AddTask(t);
  }
  else
  {
    resPtr->InternalLoad(fp);
  }
}
