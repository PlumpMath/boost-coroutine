
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_GET_H
#define BOOST_CORO_DETAIL_COROUTINE_GET_H

#include <boost/assert.hpp>
#include <boost/config.hpp>

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/param_type.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_get
{
    typename param_type< Result >::type get() const
    { return static_cast< D const* >( this)->impl_->result_.get(); }
};

template< typename Signature, typename D, int arity >
struct coroutine_get< Signature, D, void, arity >
{};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_GET_H
