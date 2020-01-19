/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/resource.h"

template <typename ... types>
class reswoman
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
  reswoman();
public:
  template <typename resource_type>
  resource_ptr<resource_type> get(const str_t &);

private:
  //Performs all the static checks for all the structural requirements a resource must meet
  template <typename ... types>
  void check_resource_type();
  //Checks if a resource is registered
  template <typename resource_type>
  void check_if_registered();
private:
  std::tuple<resource_container<types>...> m_resources;
};

template<typename ...types>
template<typename resource_type>
inline resource_ptr<resource_type> reswoman<types...>::get(const str_t & st)
{

  check_resource_type<resource_type>();

  using ct_type = resource_container<resource_type>;
  ct_type & ct = std::get<ct_type>(m_resources);


  resource_ptr<resource_type> ret_ptr;//null
  
  auto it = ct.find(resource::compute_id<resource_type>(st));
  if (it == ct.end())
  {
    //Resource not found
    //m_log.warning("get: No load function has been called with the given parameters");
    return ret_ptr;
  }
  //Found
  return it->second.m_ptr;
}




template<typename ...types>
template<typename ...in_types>
inline void reswoman<types...>::check_resource_type()
{
  static_assert(std::is_base_of<resource, in_types...>::value,
    "Every resource must derive from the resource class");
  static_assert(std::is_default_constructible<in_types...>::value,
    "Every resource must have a default constructor");
}

#define checkT(check_fun) \
template<typename T, typename N, typename ...args>\
constexpr bool checkT()\
{\
  if constexpr (std::check_fun<T, N>::value)\
    return true;\
  else\
    return checkT<T, args...>();\
}\
template<typename T>\
constexpr bool checkT()\
{\
  return false;\
}

template<typename ...types>
template<typename resource_type>
inline void reswoman<types...>::check_if_registered()
{
  static_assert(checkT<resource_type, types...>(), "The resource is not registered");
}

template<typename ...types>
inline reswoman<types...>::reswoman()
{
  check_resource_type<types...>();
}

/*
#include <type_traits>

template<typename T, typename U, typename ... Args>
constexpr bool CheckT(){
    if constexpr (std::is_same<T,U>()){
        return true;
    }else if constexpr(sizeof ... (Args) > 0)
        return CheckT<T, Args...>();

    // Base
    return false;
}

template<typename T, typename ... Args>
struct Check{
    template<typename ... Args2>
    static constexpr bool Within(){
    if constexpr (CheckT<T, Args2 ...>()){
        return true;
    }else if constexpr(sizeof ... (Args) > 0)
        return Check<Args...>::template Within<Args2...>();

    // Base
    return false;
    }
};

#define TOKEN(Name) struct Name {};
#define ASSERT(...) static_assert(__VA_ARGS__,"");

TOKEN(A);
TOKEN(B);
TOKEN(C);
TOKEN(D);
TOKEN(E);
TOKEN(F);

ASSERT(CheckT<A, B>() == false)
ASSERT(CheckT<A, B, A, C>() == true)
ASSERT(Check<A>::Within<B,C,A>() == true)
ASSERT(Check<E, A>::Within<B,C,A>() == true)


*/