#include "pch.h"

#include "rtti/type.h"

namespace RTTI
{

  type::type(size_t val)
    : m_is_type_index(false)
    , m_as_val(val)
    , m_as_index(typeid(int))
  {
    XASSERT(false);
  }

  type::type(std::type_index index)
    : m_is_type_index(true)
    , m_as_index(index)
    , m_as_val(0)
  {}

  bool type::operator==(size_t i) const
  {
    return get_hash() == i;
  }

  bool type::operator==(std::type_index i) const
  {
    XASSERT(m_is_type_index);
    return i == m_as_index;
  }

  bool type::operator==(const type & t) const
  {
    return get_hash() == t.get_hash();
  }

  std::type_index type::as_type_index() const
  {
    XASSERT(m_is_type_index);
    return m_as_index;
  }

  size_t type::get_hash() const
  {
    if (m_is_type_index)
    {
      return m_as_index.hash_code();
    }
    else
    {
      return m_as_val;
    }
  }
}