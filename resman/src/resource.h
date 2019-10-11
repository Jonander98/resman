#pragma once

#include "pch.h"
#include "utils/filepath.h"


/*
Usage
Derive + implement load
*/
class Resource
{
  friend class ResourceMan;//TODO change by another name
public:
  //No public functions for the resource except the constructor/destructor
  //It is an interface for the Resource child. Not for te Resource user
  virtual ~Resource() = default;
  Resource() = default;
  Resource(const Resource &) = delete;
  Resource & operator=(const Resource &) = delete;

protected:


  //Can only be implemented. Not called
  virtual bool load(const FilePath&) = 0;

private:

};
