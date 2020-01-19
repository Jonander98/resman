/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "resource.h"
#include "utils/message_log.h"

class resman
{
private://Private types
  template <typename resource_type>
  struct resource_node
  {
    resource_type m_resource;
    resource_ptr<resource_type> m_ptr;
  };
  template <typename resource_type>
  using resource_container = std::map<resource::id_type, resource_node<resource_type>>;
public:
  ~resman();
  resman() = default;
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
  void load(const file_path &);
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
public:
  /*
  * Sets the log used to inform the user of possible problems
  */
  void set_log(const message_log &);
  /*
  * Returns the log in its current status
  */
  const message_log & get_log()const;
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
  void check_resource_type();
private:
  //A map from type id to the corresponding resource container
  std::map<size_t, void*> m_resources;
  //The log for the errors
  message_log m_log;
};

#include "resman.inl"
