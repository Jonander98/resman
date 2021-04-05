#pragma once

namespace RTTI
{
  //TODO: redo this in a decent way
  class Type
  {
  public:
    Type(size_t val);
    Type(std::type_index index);
    Type(const Type&) = default;

    bool operator==(size_t) const;
    bool operator==(std::type_index) const;
    bool operator==(const Type &) const;
    Type& operator=(const Type&) = default;

    std::type_index AsTypeIndex() const;

    size_t GetHash() const;

  private:
    bool m_isTypeIndex;
    std::type_index m_asIndex;
    size_t m_asVal;
  };
}

namespace std
{
  template<> struct hash<RTTI::Type>
  {
    size_t operator()(RTTI::Type const& t) const noexcept
    {
      return std::hash<std::type_index>{}(t.AsTypeIndex());
    }
  };
}