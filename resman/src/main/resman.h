/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "resource.h"

class resman
{
  template <typename resource_type>
  using resource_container = std::map<resource::id_type, resource_type>;
public:
  ~resman();
  resman() = default;
  resman(const resman &) = delete;
  resman & operator=(const resman &) = delete;
public:
  /*
  * If the load for the resource has been requeted, a valid pointer to the resource is given.
  * But it might be not loaded yet.
  * If the id is not found, a non valid resource_ptr is returned
  */
  template <typename resource_type>
  resource_ptr<resource_type> get(const str_t &);
public:
  //Registers a resource type
  template <typename resource_type>
  void register_resource();
  //Registers a group of resource types
  template <typename resource_type, typename resource_type2, typename ...args>
  void register_resource();
  template <typename resource_type>
  bool is_registered();
private:

  //A map from type id to the corresponding resource container
  std::map<size_t, void*> m_resources;

};

#include "resman.inl"
