/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "resource.h"
#include "utils/message_log.h"
#include "work_group.h"

class Resman
{
public://Public types
  struct Config
  {
    //Maximum number of threads allowed
    u8 maxThreads{ 4 };
    /*
    * Minimum number of resources that have to be in the queue of a thread to create a new one
    * always respecting the maximum
    */
    u8 minResourcesToFork{ 3 };
  };
private://Private types
  template <typename resource_type>
  using resource_container = std::unordered_map<AResource::IdType, ResourcePtr<resource_type>>;
public:
  //Makes sure all the resources get unloaded
  ~Resman();
  Resman(Config = Config());
  Resman(const Resman &) = delete;
  Resman & operator=(const Resman &) = delete;
public:
  /*
  * If the load for the resource has been requested, a valid pointer to the resource is given.
  * But it might be not loaded yet.
  * If the id is not found, a non valid resource_ptr is returned
  */
  template <typename resource_type>
  ResourcePtr<resource_type> Get(const str_t &);  
  /*
  * Returns a vector with a pointer to all the resources of the given
  * type that have been loaded
  */
  template <typename resource_type>
  std::vector<ResourcePtr<resource_type>> GetAllOfType();
  /*
  * Loads a resource of the given type from the given path
  */
  template <typename resource_type>
  void Load(const Filepath &);
  /*
  * Loads a resource of the given type from the given path asyncronously
  */
  template <typename resource_type>
  void LoadAsync(const Filepath &);
  /*
  * Unloads the specified resource
  */
  template<typename resource_type>
  void Unload(const str_t &);
  /*
  * Unloads all the resources of the specified type
  */
  template<typename resource_type>
  void UnloadAllOfType();
  /*
  * Unloads all the resources
  */
  void UnloadAll();
public://log and config
  /*
  * Sets the config of the resource manager
  */
  void SetConfig(Config c);
  /*
  * Sets the log used to inform the user of possible problems
  */
  void SetLog(const MessageLog &);
  /*
  * Returns the log in its current status
  */
  const MessageLog & GetLog()const;
  /*
  * Prints information about the current resource manager state to the log
  */
  const MessageLog & GetResourceManagerStatus();
  /*
  * Saves the current status of the resource manager to a file.
  * onlyEverUsed: if true, only resources that have been used at some point will be saved
  */
  void SaveToFile(const Filepath &, bool onlyEverUsed = false)const;
  /*
  * Loads a resourcemanager status from file.
  * The types that were not registered will get ignored
  */
  void FromFile(const Filepath&);
  /*
  * Loads a resourcemanager status from file.
  * The template parameters are the types that want to be loaded from the file. If a type is not provided
  * and it is found on the file, it is ignored
  */
  template<typename ...resource_types>
  void FromFileRestrictedTypes(const Filepath&);
public:
  //Registers a resource type
  template <typename resource_type>
  void RegisterResource();
  //Registers a group of resource types
  template <typename resource_type, typename resource_type2, typename ...args>
  void RegisterResource();
  //Checks if a resource is registered
  template <typename resource_type>
  bool IsRegistered();
private:
  /*
  * Returns a pointer to the corresponding resource container.
  * It returns null if the resource was not previously registered
  */
  template <typename resource_type>
  resource_container<resource_type>* GetResourceContainer();
  //Performs all the static checks for all the structural requirements a resource must meet
  template <typename resource_type>
  constexpr void CheckResourceType();
  /*
  * Used for resource loading from file. It loads the resource if the id
  * matches with any of the types. Otherwise, nothing happens
  */
  template <typename resource_type, typename resource_type2, typename ...args>
  void LoadIfSameType(RTTI::Type id, const Filepath& fp);
  template<typename resource_type>
  void LoadIfSameType(RTTI::Type id, const Filepath& fp);
  //Internal helper version
  bool IsRegistered(RTTI::Type typeId);

  /*
  * Loads a resource of the given type from the given path.
  * Must specify if it should be asyncronous or not
  */
  template <typename resource_type>
  void InternalLoad(const Filepath &, bool is_async);

  //Helper to reuse file loading code
  void ForEachResourceInFile(const Filepath& fp, std::function<void(const Filepath&, size_t)> action) const;

private:
  //A map from type id to the corresponding resource container
  std::unordered_map<RTTI::Type, void*> m_resources;
  //The log for the messages
  mutable MessageLog m_log;
  //The work group that will load asyncronously
  WorkScheduling::WorkGroup m_workGroup;
  //The config of the resource manager(related with threads)
  Config m_config;
};

#include "resman.inl"
