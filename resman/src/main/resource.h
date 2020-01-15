/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "utils/filepath.h"

/*
Usage
Derive + implement load
*/
class resource
{
  friend class resman;
protected:
  virtual ~resource() = default;
  resource() = default;
public:
  resource(const resource &) = delete;
  resource & operator=(const resource &) = delete;

  bool is_loaded()const;
protected:


  //Can only be implemented. Not called
  virtual bool load(const file_path&) = 0;
private:
  void internal_load(const file_path&);
private:
  bool m_is_loaded{ false };  //If the resource is loaded or not
};


//shared_ptr style class
template <typename resource_type>
class resource_ptr
{
  friend class resman;
public:
  resource_ptr() = default;
  resource_ptr(const resource_ptr &);
  resource_type * get();
  resource_type * operator->()const;
  resource_ptr & operator=(const resource_ptr & rhs);
  operator resource_type *();
public:
  //returns true if the pointer is valid
  bool is_valid();


private:
  resource_type * m_ptr{nullptr};
};

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

