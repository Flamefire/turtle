// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2008
// Copyright 2020-2026 Alexander Grund
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_DETAIL_CONSTRAINT_HPP_INCLUDED
#define MOCK_DETAIL_CONSTRAINT_HPP_INCLUDED

#include "../config.hpp"
#include "../log.hpp"
#include "../unwrap_reference.hpp"
#include <boost/preprocessor/array.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <functional>
#include <type_traits>

namespace mock { namespace detail {
    template<typename Lhs, typename Rhs>
    class and_
    {
    public:
        and_(const Lhs& lhs, const Rhs& rhs) : lhs_(lhs), rhs_(rhs) {}
        template<typename Actual>
        bool operator()(const Actual& actual) const
        {
            return lhs_(actual) && rhs_(actual);
        }
        friend std::ostream& operator<<(std::ostream& s, const and_& a)
        {
            return s << "( " << mock::format(a.lhs_) << " && " << mock::format(a.rhs_) << " )";
        }

    private:
        Lhs lhs_;
        Rhs rhs_;
    };

    template<typename Lhs, typename Rhs>
    class or_
    {
    public:
        or_(const Lhs& lhs, const Rhs& rhs) : lhs_(lhs), rhs_(rhs) {}
        template<typename Actual>
        bool operator()(const Actual& actual) const
        {
            return lhs_(actual) || rhs_(actual);
        }
        friend std::ostream& operator<<(std::ostream& s, const or_& o)
        {
            return s << "( " << mock::format(o.lhs_) << " || " << mock::format(o.rhs_) << " )";
        }

    private:
        Lhs lhs_;
        Rhs rhs_;
    };

    template<typename Constraint>
    class not_
    {
    public:
        explicit not_(const Constraint& c) : c_(c) {}
        template<typename Actual>
        bool operator()(const Actual& actual) const
        {
            return !c_(actual);
        }
        friend std::ostream& operator<<(std::ostream& s, const not_& n) { return s << "! " << mock::format(n.c_); }

    private:
        Constraint c_;
    };
}} // namespace mock::detail

#define MOCK_CONSTRAINT_ASSIGN(z, n, d) expected##n(std::forward<T##n>(e##n))

#define MOCK_CONSTRAINT_UNWRAP_REF(z, n, d) mock::unwrap_ref(expected##n)

#define MOCK_CONSTRAINT_FORMAT(z, n, d) BOOST_PP_IF(n, << ", " <<, ) mock::format(c.expected##n)

#define MOCK_CONSTRAINT_MEMBER(z, n, d) Expected_##n expected##n;

#define MOCK_CONSTRAINT_TPL_TYPE(z, n, d) std::decay_t<const T##n>

#define MOCK_CONSTRAINT_CREF_PARAM(z, n, Args) \
    const mock::unwrap_reference_t<Expected_##n>& BOOST_PP_ARRAY_ELEM(n, Args)

#define MOCK_CONSTRAINT_ARG(z, n, Args) T##n&& BOOST_PP_ARRAY_ELEM(n, Args)

#define MOCK_CONSTRAINT_ARGS(z, n, Args) T##n&& e##n

#define MOCK_CONSTRAINT_PARAM(z, n, Args) std::forward<T##n>(BOOST_PP_ARRAY_ELEM(n, Args))

#define MOCK_CONSTRAINT_AUX_AUX(Name, n, Array) \
    MOCK_CONSTRAINT_EXT(Name, n, BOOST_PP_ARRAY_TO_TUPLE(BOOST_PP_ARRAY_POP_BACK(Array)), BOOST_PP_ARRAY_ELEM(n, Array))

#define MOCK_CONSTRAINT_AUX(Name, Size, Tuple) MOCK_CONSTRAINT_AUX_AUX(Name, BOOST_PP_DEC(Size), (Size, Tuple))

#endif // MOCK_DETAIL_CONSTRAINT_HPP_INCLUDED
