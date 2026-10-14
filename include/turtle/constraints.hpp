// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_CONSTRAINTS_HPP_INCLUDED
#define MOCK_CONSTRAINTS_HPP_INCLUDED

#include "config.hpp"
#include "constraint.hpp"
#include "detail/constraints.hpp"
#include <functional>
#include <memory>

namespace mock {
MOCK_UNARY_CONSTRAINT(any, 0, , ((void)actual, true))
MOCK_UNARY_CONSTRAINT(affirm, 0, , !!actual)
MOCK_UNARY_CONSTRAINT(negate, 0, , !actual)
MOCK_UNARY_CONSTRAINT(evaluate, 0, , actual())

MOCK_NARY_CONSTRAINT(less, 1, (expected), actual < expected)
MOCK_NARY_CONSTRAINT(greater, 1, (expected), actual > expected)
MOCK_NARY_CONSTRAINT(less_equal, 1, (expected), actual <= expected)
MOCK_NARY_CONSTRAINT(greater_equal, 1, (expected), actual >= expected)

#ifdef small
#    pragma push_macro("small")
#    undef small
#    define MOCK_SMALL_DEFINED
#endif
MOCK_NARY_CONSTRAINT(small, 1, (tolerance), detail::is_small(actual, tolerance))
#ifdef MOCK_SMALL_DEFINED
#    pragma pop_macro("small")
#endif

MOCK_NARY_CONSTRAINT(close,
                     2,
                     (expected, tolerance),
                     detail::is_close(actual, expected, detail::percent_tolerance(tolerance)))

MOCK_NARY_CONSTRAINT(close_fraction, 2, (expected, tolerance), detail::is_close(actual, expected, tolerance))

#ifdef near
#    pragma push_macro("near")
#    undef near
#    define MOCK_NEAR_DEFINED
#endif
MOCK_NARY_CONSTRAINT(near, 2, (expected, tolerance), std::abs(actual - expected) <= tolerance)
#ifdef MOCK_NEAR_DEFINED
#    pragma pop_macro("near")
#endif

/// Constraint that compares the actual value to the expected value using operator==
template<typename T>
constraint<detail::equal<T>> equal(T&& t)
{
    return detail::equal<T>(std::forward<T>(t));
}

template<typename T>
constraint<detail::same<T>> same(T& t)
{
    return detail::same<T>(t);
}
template<typename T>
constraint<detail::retrieve<T>> retrieve(T& t)
{
    return detail::retrieve<T>(t);
}
template<typename T>
constraint<detail::assign<T>> assign(T t)
{
    return detail::assign<T>(t);
}
template<typename T>
constraint<detail::contain<T>> contain(T t)
{
    return detail::contain<T>(t);
}

template<typename T>
constraint<T> call(T t)
{
    return constraint<T>(t);
}
} // namespace mock

#endif // MOCK_CONSTRAINTS_HPP_INCLUDED
