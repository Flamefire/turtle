// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_MATCHER_HPP_INCLUDED
#define MOCK_MATCHER_HPP_INCLUDED

#include "config.hpp"
#include "constraints.hpp"
#include "detail/is_functor.hpp"
#include "detail/ref_arg.hpp"
#include "log.hpp"
#include <cstring>
#include <functional>
#include <type_traits>

namespace mock {
/// Matches an actual argument of a mock call against an expected value
template<typename Actual, typename Expected, typename Enable = void>
class matcher
{
public:
    explicit matcher(Expected expected) : expected_(expected) {}
    /// Return true if the actual value satisfies this matcher
    bool operator()(std::add_lvalue_reference_t<const Actual> actual)
    {
        return mock::equal(mock::unwrap_ref(expected_)).c_(actual);
    }
    friend std::ostream& operator<<(std::ostream& s, const matcher& m) { return s << mock::format(m.expected_); }

private:
    Expected expected_;
};

/// Compares two C-strings (null-safe) by value
template<>
class matcher<const char*, const char*>
{
public:
    explicit matcher(const char* expected) : expected_(expected) {}
    /// Return true if the actual value satisfies this matcher
    bool operator()(const char* actual)
    {
        if(!actual || !expected_)
            return actual == expected_;
        return std::strcmp(actual, expected_) == 0;
    }
    friend std::ostream& operator<<(std::ostream& s, const matcher& m) { return s << mock::format(m.expected_); }

private:
    const char* expected_;
};

/// Delegates matching to a user-supplied mock::constraint
template<typename Actual, typename Constraint>
class matcher<Actual, mock::constraint<Constraint>>
{
public:
    explicit matcher(const constraint<Constraint>& c) : c_(c.c_) {}
    /// Return true if the actual value satisfies this matcher
    bool operator()(detail::ref_arg_t<Actual> actual) { return c_(static_cast<detail::ref_arg_t<Actual>>(actual)); }
    friend std::ostream& operator<<(std::ostream& s, const matcher& m) { return s << mock::format(m.c_); }

private:
    Constraint c_;
};

/// Delegates matching to a user-supplied functor
template<typename Actual, typename Functor>
class matcher<Actual, Functor, std::enable_if_t<detail::is_functor<Functor, Actual>::value>>
{
public:
    explicit matcher(const Functor& f) : c_(f) {}
    /// Return true if the actual value satisfies this matcher
    bool operator()(detail::ref_arg_t<Actual> actual) { return c_(static_cast<detail::ref_arg_t<Actual>>(actual)); }
    friend std::ostream& operator<<(std::ostream& s, const matcher& m) { return s << mock::format(m.c_); }

private:
    Functor c_;
};
} // namespace mock

#endif // MOCK_MATCHER_HPP_INCLUDED
