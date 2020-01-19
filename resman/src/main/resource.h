/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "utils/filepath.h"

/*
Hash function used to compute the id
It can be specialized for a concrete resource
*/
template <typename resource_type>
struct resource_hash : public std::hash<str_t> {};
/*
Usage
Derive + implement load
*/
class resource
{
private://types
  friend class resman;
  template <typename ...types>
  friend class reswoman;
  using id_type = size_t;
public://Static interface
  template <typename resource_type>
  static id_type compute_id(const str_t &);
protected:
  virtual ~resource() = default;
  resource();
  //Only allow moving, not copying
  resource(const resource &) = delete;
  resource(resource &&) = default;
  //Cannot be assigned to avoid that the user duplicates data
  resource & operator=(const resource &) = delete;
  resource & operator=(resource &&) = default;
public:
  //True if the resource has been loaded
  bool is_loaded()const;
  //The id of the resource
  id_type get_id()const;
protected:

  //Override with the load of the resource
  virtual bool load(const file_path&) = 0;
  //Option to control when a resource is unloaded apart from the destructor
  virtual void unload();
private:
  //Wrapper around load to enable performing actions related with loading
  void internal_load(const file_path&);
  //Wrapper around unload to enable performing actions related with unloading
  void internal_unload();
private:
  bool m_is_loaded{ false };  //If the resource is loaded or not
  id_type m_id;               //Identification number for the resource
};


/*
*  A pointer to a resource
*  It keeps track of how many of them exist as shared ptr, but the resman has control over
*  when the dealocation of the resource happens
*/
template <typename resource_type>
class resource_ptr
{
  static_assert(std::is_base_of<resource, resource_type>::value,
    "Every resource must derive from the resource class");

  friend class resman;
  //Only the resource manager can create valid versions of these
  explicit resource_ptr(resource_type * );
public:
  //Allow for default contruction
  resource_ptr();
  ~resource_ptr();
  resource_ptr(const resource_ptr &);
  //Returns the raw pointer
  resource_type * get();
  //Applies the arrow operator on the raw pointer
  resource_type * operator->()const;
  //Assignment operator between two resource ptr
  resource_ptr & operator=(const resource_ptr & rhs);
  //Allows using the resource_ptr as a raw pointer
  operator resource_type *();
public:
  //Returns true if the pointer is valid
  bool is_valid()const;
  //Returns the number of times the pointer is referenced
  i32 get_reference_count()const;

private:
  //Removes the ownership from its pointed resource
  void remove_ownership();
private:
  i32 * m_ref_count;    //Number of active references to the pointed value
  resource_type * m_ptr;//A pointer to the data
};

#include "resource.inl"