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
/// Wraps a constraint functor; constraints can be combined with &&, || and negated with !
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
} // namespace mock

#define MOCK_UNARY_CONSTRAINT(Name, n, Args, Expr)                                                               \
    namespace detail {                                                                                           \
        struct Name                                                                                              \
        {                                                                                                        \
            template<typename Actual>                                                                            \
            bool operator()(const Actual& actual) const                                                          \
            {                                                                                                    \
                (void)actual;                                                                                    \
                return Expr;                                                                                     \
            }                                                                                                    \
            friend std::ostream& operator<<(std::ostream& s, const Name&) { return s << BOOST_STRINGIZE(Name); } \
        };                                                                                                       \
    }                                                                                                            \
    const mock::constraint<detail::Name> Name;

#define MOCK_NARY_CONSTRAINT(Name, n, Args, Expr)                                                                   \
    namespace detail {                                                                                              \
        template<BOOST_PP_ENUM_PARAMS(n, typename Expected_)>                                                       \
        struct Name                                                                                                 \
        {                                                                                                           \
            template<BOOST_PP_ENUM_PARAMS(n, typename T)>                                                           \
            explicit Name(BOOST_PP_ENUM(n, MOCK_CONSTRAINT_ARGS, _)) : BOOST_PP_ENUM(n, MOCK_CONSTRAINT_ASSIGN, _)  \
            {}                                                                                                      \
            template<typename Actual>                                                                               \
            bool operator()(const Actual& actual) const                                                             \
            {                                                                                                       \
                return test(actual, BOOST_PP_ENUM(n, MOCK_CONSTRAINT_UNWRAP_REF, _));                               \
            }                                                                                                       \
            template<typename Actual>                                                                               \
            bool test(const Actual& actual, BOOST_PP_ENUM(n, MOCK_CONSTRAINT_CREF_PARAM, (n, Args))) const          \
            {                                                                                                       \
                return Expr;                                                                                        \
            }                                                                                                       \
            friend std::ostream& operator<<(std::ostream& s, const Name& c)                                         \
            {                                                                                                       \
                return s << BOOST_STRINGIZE(Name) << "( " << BOOST_PP_REPEAT(n, MOCK_CONSTRAINT_FORMAT, _) << " )"; \
            }                                                                                                       \
            BOOST_PP_REPEAT(n, MOCK_CONSTRAINT_MEMBER, _)                                                           \
        };                                                                                                          \
    }                                                                                                               \
    template<BOOST_PP_ENUM_PARAMS(n, typename T)>                                                                   \
    mock::constraint<detail::Name<BOOST_PP_ENUM(n, MOCK_CONSTRAINT_TPL_TYPE, _)>> Name(                             \
      BOOST_PP_ENUM(n, MOCK_CONSTRAINT_ARG, (n, Args)))                                                             \
    {                                                                                                               \
        return detail::Name<BOOST_PP_ENUM(n, MOCK_CONSTRAINT_TPL_TYPE, _)>(                                         \
          BOOST_PP_ENUM(n, MOCK_CONSTRAINT_PARAM, (n, Args)));                                                      \
    }

/// Define a custom constraint named `Name`, taking `n` arguments with names given as `Args`-tuple
/// and evaluates the given expression on the actual argument
/// MOCK_CONSTRAINT_EXT(Name, (argument1, argument2, ...), expression)
#define MOCK_CONSTRAINT_EXT(Name, n, Args, Expr) \
    BOOST_PP_IF(n, MOCK_NARY_CONSTRAINT, MOCK_UNARY_CONSTRAINT)(Name, n, Args, Expr)

/// Define a custom constraint named `Name` evaluated by the given expression on the actual argument
/// MOCK_CONSTRAINT(Name, [argument1, argument2, ...], expression)
#define MOCK_CONSTRAINT(Name, ...) MOCK_CONSTRAINT_AUX(Name, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), (__VA_ARGS__))

#endif // MOCK_CONSTRAINT_HPP_INCLUDED
