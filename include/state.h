#ifndef MMPL_STATE_H
#define MMPL_STATE_H

// C++ Standard Library
#include <cstdint>
#include <functional>
#include <type_traits>

// MMPL
#include <mmpl/crtp.h>

namespace mmpl
{

template<typename T>
struct StateTraits;


template<typename StateT>
using state_id_t = typename StateTraits<StateT>::IDType;


template<typename DerivedT>
struct StateBase
{
public:
  using IDType = state_id_t<DerivedT>;

  inline IDType id() const
  {
    return this->derived()->id_impl();
  }

  inline bool operator==(const DerivedT& other) const
  {
    return this->derived()->equals_impl(other);
  }

private:
  IMPLEMENT_CRTP_BASE_CLASS(StateBase, DerivedT);
};


template<typename T>
using state_default_hash_t = ::std::hash<StateBase<T>>;


template<typename StateT>
struct is_state : std::integral_constant<bool, std::is_base_of<StateBase<StateT>, StateT>::value> {};

}  // namespace mmpl


namespace std
{

template<typename StateT>
struct hash<::mmpl::StateBase<StateT>>
{
  static_assert(std::is_convertible<::mmpl::state_id_t<StateT>, std::size_t>(),
                "Opaque ID-type associated StateT must be convertible to std::size_t");

  inline std::size_t operator()(const ::mmpl::StateBase<StateT>& state) const
  {
    return state.id();
  }
};

}  // namespace mmpl

#endif  // MMPL_STATE_H
