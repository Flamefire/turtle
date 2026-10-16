// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2008
// Copyright 2020-2025 Alexander Grund
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_CONSTRAINT_HPP_INCLUDED
#define MOCK_CONSTRAINT_HPP_INCLUDED

#include "config.hpp"
#include "detail/constraint.hpp"
#include <boost/preprocessor/array.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <functional>
#include <type_traits>

namespace mock {
/// Wraps a constraint functor; constraints can be combined with &&, ||, dereferenced with * and negated with !
template<typename Constraint>
struct constraint
{
    constraint() {}
    constraint(const Constraint& c) : c_(c) {}
    Constraint c_;
};

/// Combine two constraints so the result passes when either operand passes
template<typename Lhs, typename Rhs>
const constraint<detail::or_<Lhs, Rhs>> operator||(const constraint<Lhs>& lhs, const constraint<Rhs>& rhs)
{
    return detail::or_<Lhs, Rhs>(lhs.c_, rhs.c_);
}

/// Combine two constraints so the result passes when both operands pass
template<typename Lhs, typename Rhs>
const constraint<detail::and_<Lhs, Rhs>> operator&&(const constraint<Lhs>& lhs, const constraint<Rhs>& rhs)
{
    return detail::and_<Lhs, Rhs>(lhs.c_, rhs.c_);
}

/// Negate a constraint so the result passes when the original fails
template<typename Constraint>
const constraint<detail::not_<Constraint>> operator!(const constraint<Constraint>& c)
{
    return detail::not_<Constraint>(c.c_);
}

/// Apply a constraint to the value the actual argument points to, i.e. match pointer-like
/// arguments (raw or smart pointers) by their pointee. A null actual never passes.
template<typename Constraint>
const constraint<detail::pointee<Constraint>> operator*(const constraint<Constraint>& c)
{
    return detail::pointee<Constraint>(c.c_);
}
} // namespace mock

/// Define a custom constraint named `Name`, taking `n` parameters with names given as `Args`-tuple
/// and evaluates the given expression on the actual argument
/// MOCK_CONSTRAINT_EXT(Name, (param1, param2, ...), expression)
#define MOCK_CONSTRAINT_EXT(Name, n, Args, Expr) \
    BOOST_PP_IF(n, MOCK_NARY_CONSTRAINT, MOCK_UNARY_CONSTRAINT)(Name, n, Args, Expr)

/// Define a custom constraint named `Name` evaluated by the given expression on the actual argument
/// MOCK_CONSTRAINT(Name, [param1, param2, ...], expression)
///
/// It can take any number of parameters to be used by the expression.
/// E.g. `MOCK_CONSTRAINT(in_range, min, max, actual >= min && actual <= max)`
/// which can then be used like `MOCK_EXPECT(...).with(in_range(5, 10))`
#define MOCK_CONSTRAINT(Name, ...) MOCK_CONSTRAINT_AUX(Name, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), (__VA_ARGS__))

#endif // MOCK_CONSTRAINT_HPP_INCLUDED
