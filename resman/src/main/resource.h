/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "utils/filepath.h"

/*
* Hash function used to compute the id
* It can be specialized for a concrete resource
*/
template <typename resource_type>
struct ResourceHash : public std::hash<str_t> {};
/*
* Base for all the resources used by the resource manager
* All the resources must derive from this an implement a load function
*/
class AResource
{
private://types
  friend class Resman;
  using IdType = size_t;
public://Static interface
  template <typename resource_type>
  static IdType ComputeId(const str_t &);
protected:
  virtual ~AResource() = default;
  AResource() = default;
  //Only allow moving, not copying to avoid duplicating data
  AResource(const AResource &) = delete;
  AResource(AResource &&) = default;
  //Cannot be assigned to avoid that the user duplicates data
  AResource & operator=(const AResource &) = delete;
  AResource & operator=(AResource &&) = default;
public:
  //True if the resource has been loaded
  bool IsLoaded()const;
  //The id of the resource
  IdType GetId()const;
protected:

  //Override with the load of the resource
  virtual bool VLoad(const Filepath&) = 0;
  //Option to control when a resource is unloaded apart from the destructor
  virtual void VUnload();
private:
  //Wrapper around load to enable performing actions related with loading
  void InternalLoad(const Filepath&);
  //Wrapper around unload to enable performing actions related with unloading
  void InternalUnload();
private:
  //If the resource is loaded or not
  bool m_isLoaded{ false };
  //If the resource has been used at some point
  bool m_anyUse{ false };
  //Identification number for the resource
  IdType m_id;
  //The path from which the resource has been loaded
  Filepath m_path;
};


/*
*  A pointer to a resource
*  It keeps track of how many of them exist as shared ptr, but the resman has control over
*  when the dealocation of the resource happens
*/
template <typename resource_type>
class ResourcePtr
{
  static_assert(std::is_base_of<AResource, resource_type>::value,
    "Every resource must derive from the resource class");

  //Only the resource manager can create valid versions of these
  friend class Resman;
  explicit ResourcePtr(resource_type *);
public:
  //Allow for default contruction
  ResourcePtr();
  //Reduces the count of active references
  ~ResourcePtr();
  //Create a new reference to the resource
  ResourcePtr(const ResourcePtr &);
  //Returns the raw pointer
  resource_type * Get();
  //Applies the arrow operator on the raw pointer
  resource_type * operator->()const;
  //Assignment operator between two resource ptr
  ResourcePtr & operator=(const ResourcePtr & rhs);
  //Allows using the resource_ptr as a raw pointer
  operator resource_type *();
public:
  //Returns true if the pointer is valid
  bool IsValid()const;
  //Returns the number of times the pointer is referenced
  i32 GetReferenceCount()const;

private:
  //Removes the ownership from its pointed resource
  void RemoveOwnership();
private:
  //Number of active references to the pointed value
  std::atomic<i32> * m_refCount;    
  //A pointer to the data
  resource_type * m_ptr;
};

#include "resource.inl"