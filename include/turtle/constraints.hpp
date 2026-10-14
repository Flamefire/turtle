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
#include "unwrap_reference.hpp"
#include <functional>
#include <memory>

namespace mock {
/// Constraint that allows any value
MOCK_UNARY_CONSTRAINT(any, 0, , ((void)actual, true))
/// Constraint that passes when the actual value is truthy
MOCK_UNARY_CONSTRAINT(affirm, 0, , !!actual)
/// Constraint that passes when the actual value is falsy
MOCK_UNARY_CONSTRAINT(negate, 0, , !actual)
/// Constraint that invokes a functor without arguments and passes if it returns true
MOCK_UNARY_CONSTRAINT(evaluate, 0, , actual())

/// Constraint that passes when the actual value is less than the expected value (operator<)
MOCK_NARY_CONSTRAINT(less, 1, (expected), actual < expected)
/// Constraint that passes when the actual value is greater than the expected value (operator>)
MOCK_NARY_CONSTRAINT(greater, 1, (expected), actual > expected)
/// Constraint that passes when the actual value is less than or equal to the expected value
MOCK_NARY_CONSTRAINT(less_equal, 1, (expected), actual <= expected)
/// Constraint that passes when the actual value is greater than or equal to the expected value
MOCK_NARY_CONSTRAINT(greater_equal, 1, (expected), actual >= expected)

#ifdef small
#    pragma push_macro("small")
#    undef small
#    define MOCK_SMALL_DEFINED
#endif
/// Constraint that passes when the value is close to zero, i.e. |actual| < tolerance
MOCK_NARY_CONSTRAINT(small, 1, (tolerance), detail::is_small(actual, tolerance))
#ifdef MOCK_SMALL_DEFINED
#    pragma pop_macro("small")
#endif

/// Constraint that passes when the value is close to the expected value with a tolerance
/// given as a percentage of the expected value,
/// i.e. |actual - expected| / |expected| <= tolerance / 100
MOCK_NARY_CONSTRAINT(close,
                     2,
                     (expected, tolerance),
                     detail::is_close(actual, expected, detail::percent_tolerance(tolerance)))

/// Constraint that passes when the value is close to the expected value with a tolerance
/// given as a fraction of the expected value,
/// i.e. |actual - expected| / |expected| <= tolerance
MOCK_NARY_CONSTRAINT(close_fraction, 2, (expected, tolerance), detail::is_close(actual, expected, tolerance))

#ifdef near
#    pragma push_macro("near")
#    undef near
#    define MOCK_NEAR_DEFINED
#endif
/// Constraint that passes when the value is close to the expected value with a absolute tolerance
/// i.e. |actual - expected| <= tolerance
MOCK_NARY_CONSTRAINT(near, 2, (expected, tolerance), std::abs(actual - expected) <= tolerance)
#ifdef MOCK_NEAR_DEFINED
#    pragma pop_macro("near")
#endif

/// Constraint that compares the actual value to the expected value using operator==
template<typename T>
constraint<detail::equal<T>> equal(T&& expected)
{
    return detail::equal<T>(std::forward<T>(expected));
}

/// Constraint that compares the actual value to the expected value by address,
/// i.e. `&actual == &expected after removing reference wrappers
template<typename T>
constraint<detail::same<T>> same(T& expected)
{
    return detail::same<T>(expected);
}

/// \brief Constraint that copies the actual value into the given variable
///
/// \warning This may modify \ref out in unexpected ways.
/// For instance it may be called again after the expectations have already been exhausted because of the way the
/// selection algorithm works. Therefore it is probably a good idea to use an \ref action instead.
template<typename T>
constraint<detail::retrieve<T>> retrieve(T& out)
{
    return detail::retrieve<T>(out);
}

/// Constraint that assigns the value to the actual parameter
///
/// \warning This may modify the passed (actual) argument in unexpected ways.
/// For instance it may be called again after the expectations have already been exhausted because of the way the
/// selection algorithm works. Therefore it is probably a good idea to use an \ref action instead.
template<typename T>
constraint<detail::assign<T>> assign(T value)
{
    return detail::assign<T>(value);
}

/// Constraint that checks whether the actual string contains the expected substring
template<typename T>
constraint<detail::contain<T>> contain(T sub_str)
{
    return detail::contain<T>(sub_str);
}

/// Use a custom callable as a constraint to check the passed parameter.
/// It will be called with the actual value and should return true if it matches the expectation, else false.
/// E.g. the expected signature is something like `bool(const Actual&)`
template<typename Callable>
constraint<Callable> call(Callable callable)
{
    return constraint<Callable>(callable);
}
} // namespace mock

#endif // MOCK_CONSTRAINTS_HPP_INCLUDED
