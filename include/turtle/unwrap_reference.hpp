// http://turtle.sourceforge.net
//
// Copyright Alexander Grund 2020
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_UNWRAP_REFERENCE_HPP_INCLUDED
#define MOCK_UNWRAP_REFERENCE_HPP_INCLUDED

#include <functional>
#include <type_traits>

namespace mock {
/// Trait that strips std::reference_wrapper from a type, yielding the referenced type
template<class T>
struct unwrap_reference
{
    using type = T;
};
/// Strips a std::reference_wrapper to its referenced type
template<class T>
struct unwrap_reference<std::reference_wrapper<T>>
{
    using type = T;
};
/// Strips a const std::reference_wrapper to its referenced type
template<class T>
struct unwrap_reference<const std::reference_wrapper<T>>
{
    using type = T;
};
/// Type alias for unwrap_reference<T>::type
template<class T>
using unwrap_reference_t = typename unwrap_reference<T>::type;

/// Return the referenced object of a std::reference_wrapper, or the value itself otherwise
template<class T>
BOOST_FORCEINLINE unwrap_reference_t<T>& unwrap_ref(T& t) noexcept
{
    return t;
}
} // namespace mock

#endif // MOCK_UNWRAP_REFERENCE_HPP_INCLUDED
