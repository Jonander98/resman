#pragma once

#include "pch.h"
#include "utils/filepath.h"


//shared_ptr style class
template <typename resource_type>
class resource_ptr
{
  friend class resman;//TODO change by another name
public:
  //No public functions for the resource except the constructor/destructor
  //It is an interface for the Resource child. Not for te Resource user
  virtual ~resource_ptr() = default;
  resource_ptr() = default;
  resource_ptr(const resource_ptr &) = delete;
  resource_ptr & operator=(const resource_ptr &) = delete;

protected:


  //Can only be implemented. Not called
  virtual bool load(const file_path&) = 0;

private:

};

/*
Usage
Derive + implement load
*/
class resource
{
  friend class ResourceMan;//TODO change by another name
public:
  //No public functions for the resource except the constructor/destructor
  //It is an interface for the Resource child. Not for te Resource user
  virtual ~resource() = default;
  resource() = default;
  resource(const resource &) = delete;
  resource & operator=(const resource &) = delete;

protected:


  //Can only be implemented. Not called
  virtual bool load(const file_path&) = 0;

private:

};
