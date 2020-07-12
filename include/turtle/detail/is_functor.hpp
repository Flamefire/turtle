// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2009
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_IS_FUNCTOR_HPP_INCLUDED
#define MOCK_IS_FUNCTOR_HPP_INCLUDED

#include "../config.hpp"
#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace mock
{
namespace detail
{
    /// Trait to return true if F is a functor that can be called with a single argument Arg
    template< typename F, typename Arg, class = void >
    struct is_functor : std::false_type
    {};
    template< typename F, typename Arg >
    struct is_functor< F, Arg, boost::void_t<decltype( std::declval<F>()( std::declval<Arg>() ) )> >: std::true_type
    {};
}
} // mock

#endif // MOCK_IS_FUNCTOR_HPP_INCLUDED
