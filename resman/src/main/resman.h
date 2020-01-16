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
  //If the id is registered, a valid pointer to a resource is given. But it might be not loaded yet
  //If the id is not registered, null is returned
  template <typename resource_type>
  resource_ptr<resource_type> get(const str_t &);
public:
  template <typename resource_type>
  void register_resource();
  template <typename resource_type, typename resource_type2, typename ...args>
  void register_resource();
private:

  //A map from type id to the corresponding resource container
  std::map<size_t, void*> m_resources;

};

#include "resman.inl"
