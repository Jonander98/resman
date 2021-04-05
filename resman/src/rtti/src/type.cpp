#include "pch.h"

#include "rtti/type.h"

namespace RTTI
{

  Type::Type(size_t val)
    : m_isTypeIndex(false)
    , m_asVal(val)
    , m_asIndex(typeid(int))
  {
    XASSERT(false);
  }

  Type::Type(std::type_index index)
    : m_isTypeIndex(true)
    , m_asIndex(index)
    , m_asVal(0)
  {}

  bool Type::operator==(size_t i) const
  {
    return GetHash() == i;
  }

  bool Type::operator==(std::type_index i) const
  {
    XASSERT(m_isTypeIndex);
    return i == m_asIndex;
  }

  bool Type::operator==(const Type & t) const
  {
    return GetHash() == t.GetHash();
  }

  std::type_index Type::AsTypeIndex() const
  {
    XASSERT(m_isTypeIndex);
    return m_asIndex;
  }

  size_t Type::GetHash() const
  {
    if (m_isTypeIndex)
    {
      return m_asIndex.hash_code();
    }
    else
    {
      return m_asVal;
    }
  }
}