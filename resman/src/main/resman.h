/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "resource.h"
#include "utils/message_log.h"
#include "work_group.h"


class resman
{
public://Public types
  struct config
  {
    //Maximum number of threads allowed
    u8 max_threads{ 4 };
    /*
    * Minimum number of resources that have to be in the queue of a thread to create a new one
    * always respecting the maximum
    */
    u8 min_resources_to_fork{ 3 };
  };
private://Private types
  template <typename resource_type>
  using resource_container = std::map<resource::id_type, resource_ptr<resource_type>>;
public:
  //Makes sure all the resources get unloaded
  ~resman();
  resman(config = config());
  resman(const resman &) = delete;
  resman & operator=(const resman &) = delete;
public:
  /*
  * If the load for the resource has been requested, a valid pointer to the resource is given.
  * But it might be not loaded yet.
  * If the id is not found, a non valid resource_ptr is returned
  */
  template <typename resource_type>
  resource_ptr<resource_type> get(const str_t &);  
  /*
  * Returns a vector with a pointer to all the resources of the given
  * type that have been loaded
  */
  template <typename resource_type>
  std::vector<resource_ptr<resource_type>> get_all_t();
  /*
  * Loads a resource of the given type from the given path
  */
  template <typename resource_type>
  void load(const filepath &);
  /*
  * Loads a resource of the given type from the given path asyncronously
  */
  template <typename resource_type>
  void load_async(const filepath &);
  /*
  * Unloads the specified resource
  */
  template<typename resource_type>
  void unload(const str_t &);
  /*
  * Unloads all the resources of the specified type
  */
  template<typename resource_type>
  void unload_all_t();
  /*
  * Unloads all the resources
  */
  void unload_all();
public://log and config
  /*
  * Sets the config of the resource manager
  */
  void set_config(config c);
  /*
  * Sets the log used to inform the user of possible problems
  */
  void set_log(const message_log &);
  /*
  * Returns the log in its current status
  */
  const message_log & get_log()const;
  /*
  * Prints information about the current resource manager state to the log
  */
  const message_log & get_resource_manager_status();
  /*
  * Saves the current status of the resource manager to a file.
  * only_ever_used: if true, only resources that have been used at some point will be saved
  */
  void save_to_file(const filepath &, bool only_ever_used = false)const;
  /*
  * Loads a resourcemanager status from file. All the loaded resources are unloaded
  * The template parameters are the types that want to be loaded from the file. If a type is not provided
  * and it is found on the file, it is ignored
  */
  template<typename ...resource_types>
  void from_file(const filepath&);
public:
  //Registers a resource type
  template <typename resource_type>
  void register_resource();
  //Registers a group of resource types
  template <typename resource_type, typename resource_type2, typename ...args>
  void register_resource();
  //Checks if a resource is registered
  template <typename resource_type>
  bool is_registered();
private:
  /*
  * Returns a pointer to the corresponding resource container.
  * It returns null if the resource was not previously registered
  */
  template <typename resource_type>
  resource_container<resource_type> * get_resource_container();
  //Performs all the static checks for all the structural requirements a resource must meet
  template <typename resource_type>
  constexpr void check_resource_type();
  /*
  * Used for resource loading from file. It loads the resource if the id
  * matches with any of the types. Otherwise, nothing happens
  */
  template <typename resource_type, typename resource_type2, typename ...args>
  void load_if_same_type(size_t id, const filepath& fp);
  template<typename resource_type>
  void load_if_same_type(size_t id, const filepath& fp);

  /*
  * Loads a resource of the given type from the given path.
  * Must specify if it should be asyncronous or not
  */
  template <typename resource_type>
  void internal_load(const filepath &, bool is_async);

private:
  //A map from type id to the corresponding resource container
  std::map<size_t, void*> m_resources;
  //The log for the messages
  message_log m_log;
  //The work group that will load asyncronously
  work_group m_work_group;
  //The config of the resource manager(related with threads)
  config m_config;
};

#include "resman.inl"
