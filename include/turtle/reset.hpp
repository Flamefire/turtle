// http://turtle.sourceforge.net
//
// Copyright Mathieu Champlon 2012
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOCK_RESET_HPP_INCLUDED
#define MOCK_RESET_HPP_INCLUDED

#include "config.hpp"
#include "detail/functor.hpp"
#include "detail/root.hpp"
#include "object.hpp"

namespace mock {
/// Reset all pending expectations for all mock objects, functions and functors
inline void reset()
{
    detail::root.reset();
}
/// Reset all pending expectations for the given mock object and all its methods
inline void reset(const object& o)
{
    o.impl_->reset();
}
/// Reset all pending expectations for the given mock functor
template<typename Signature>
void reset(detail::functor<Signature>& f)
{
    f.reset();
}
} // namespace mock

#endif // MOCK_RESET_HPP_INCLUDED
