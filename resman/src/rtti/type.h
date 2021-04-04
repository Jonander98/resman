#pragma once

namespace RTTI
{
  //TODO: redo this in a decent way
  class type
  {
  public:
    type(size_t val);
    type(std::type_index index);
    type(const type&) = default;

    bool operator==(size_t) const;
    bool operator==(std::type_index) const;
    bool operator==(const type &) const;
    type& operator=(const type&) = default;

    std::type_index as_type_index() const;

    size_t get_hash() const;

  private:
    bool m_is_type_index;
    std::type_index m_as_index;
    size_t m_as_val;
  };
}

namespace std
{
  template<> struct hash<RTTI::type>
  {
    size_t operator()(RTTI::type const& t) const noexcept
    {
      return std::hash<std::type_index>{}(t.as_type_index());
    }
  };
}