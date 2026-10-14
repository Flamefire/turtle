// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_DETAIL_CONSTRAINTS_HPP_INCLUDED
#define MOCK_DETAIL_CONSTRAINTS_HPP_INCLUDED

#include "../log.hpp"
#include "void_t.hpp"
#include <boost/version.hpp>
#if BOOST_VERSION >= 107000
#    include <boost/test/tools/floating_point_comparison.hpp>
#else
#    include <boost/test/floating_point_comparison.hpp>
#endif
#include <type_traits>

namespace mock { namespace detail {

#if BOOST_VERSION < 105900
    using boost::test_tools::percent_tolerance;
#else
    using boost::math::fpc::percent_tolerance;
#endif

    template<typename T, typename Tolerance>
    bool is_small(const T& actual, const Tolerance& tolerance)
    {
#if BOOST_VERSION < 105900
        return boost::test_tools::check_is_small(actual, tolerance)
#else
        return boost::math::fpc::small_with_tolerance<T>(tolerance)(actual);
#endif
    }

    template<typename T1, typename T2, typename Tolerance>
    bool is_close(const T1& actual, const T2& expected, const Tolerance& tolerance)
    {
#if BOOST_VERSION < 105900
        using TolType = std::conditional_t<std::is_same<Tolerance, percent_tolerance>::value,
                                           Tolerance,
                                           boost::math::fpc::fraction_tolerance>;
        boost::test_tools::check_is_close(actual, expected, TolType(tolerance))
#else
        using common_type = std::common_type_t<T1, T2>;
        using FPT = std::conditional_t<std::is_floating_point<common_type>::value, common_type, double>;
        return boost::math::fpc::close_at_tolerance<FPT>(tolerance, boost::math::fpc::FPC_STRONG)(
          static_cast<FPT>(actual), static_cast<FPT>(expected));
#endif
    }

    template<class T, class U = T, class = void>
    struct has_equal_to : std::false_type
    {};

    template<class T, class U>
    struct has_equal_to<T, U, void_t<decltype(std::declval<T>() == std::declval<U>())>> : std::true_type
    {};

    template<class T, class U>
    constexpr bool has_equal_to_v = has_equal_to<T, U>::value;

    template<typename Expected>
    struct equal
    {
        explicit equal(Expected expected) : expected_(expected) {}
        template<typename Actual>
        bool operator()(const Actual& actual,
                        std::enable_if_t<has_equal_to_v<Actual, unwrap_reference_t<Expected>>>* = 0) const
        {
            return actual == unwrap_ref(expected_);
        }
        template<typename Actual>
        bool operator()(const Actual& actual,
                        std::enable_if_t<!has_equal_to_v<Actual, unwrap_reference_t<Expected>>>* = 0) const
        {
            return actual && *actual == unwrap_ref(expected_);
        }
        friend std::ostream& operator<<(std::ostream& s, const equal& e)
        {
            return s << "equal( " << mock::format(e.expected_) << " )";
        }
        Expected expected_;
    };

    template<typename Expected>
    class same
    {
        using ExpectedPtr = const unwrap_reference_t<Expected>*;

    public:
        explicit same(const Expected& expected) : expected_(std::addressof(unwrap_ref(expected))) {}
        template<typename Actual>
        bool operator()(const Actual& actual) const
        {
            auto actual_addr = std::addressof(actual);
            static_assert(has_equal_to_v<decltype(actual_addr), ExpectedPtr>,
                          "Type used for mock::same is incompatible with actual type");
            return actual_addr == expected_;
        }
        friend std::ostream& operator<<(std::ostream& os, const same& s)
        {
            return os << "same( " << mock::format(*s.expected_) << " )";
        }
        ExpectedPtr expected_;
    };

    template<typename Expected>
    struct retrieve
    {
        explicit retrieve(Expected& expected) : expected_(std::addressof(unwrap_ref(expected))) {}
        template<typename Actual>
        bool operator()(
          const Actual& actual,
          std::enable_if_t<!std::is_convertible<const Actual*, unwrap_reference_t<Expected>>::value>* = 0) const
        {
            *expected_ = actual;
            return true;
        }
        template<typename Actual>
        bool operator()(
          Actual&& actual,
          std::enable_if_t<!std::is_convertible<const Actual*, unwrap_reference_t<Expected>>::value>* = 0) const
        {
            *expected_ = std::move(actual);
            return true;
        }
        template<typename Actual>
        bool operator()(Actual& actual,
                        std::enable_if_t<std::is_convertible<Actual*, unwrap_reference_t<Expected>>::value>* = 0) const
        {
            *expected_ = std::addressof(actual);
            return true;
        }
        friend std::ostream& operator<<(std::ostream& s, const retrieve& r)
        {
            return s << "retrieve( " << mock::format(*r.expected_) << " )";
        }
        unwrap_reference_t<Expected>* expected_;
    };

    template<typename Expected>
    struct assign
    {
        explicit assign(const Expected& expected) : expected_(expected) {}
        template<typename Actual>
        bool operator()(Actual& actual) const
        {
            actual = unwrap_ref(expected_);
            return true;
        }
        template<typename Actual>
        bool operator()(Actual* actual,
                        std::enable_if_t<std::is_convertible<unwrap_reference_t<Expected>, Actual>::value>* = 0) const
        {
            if(!actual)
                return false;
            *actual = unwrap_ref(expected_);
            return true;
        }
        friend std::ostream& operator<<(std::ostream& s, const assign& a)
        {
            return s << "assign( " << mock::format(a.expected_) << " )";
        }
        Expected expected_;
    };

    template<typename Expected>
    struct contain
    {
        explicit contain(const Expected& expected) : expected_(expected) {}
        bool operator()(const std::string& actual) const
        {
            return actual.find(unwrap_ref(expected_)) != std::string::npos;
        }
        friend std::ostream& operator<<(std::ostream& s, const contain& n)
        {
            return s << "contain( " << mock::format(n.expected_) << " )";
        }
        Expected expected_;
    };
}} // namespace mock::detail

#endif
