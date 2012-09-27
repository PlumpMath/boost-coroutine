
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_BASE_SUSPEND_H
#define BOOST_CORO_DETAIL_COROUTINE_BASE_SUSPEND_H

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/param_type.hpp>
#include <boost/coroutine/exceptions.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_base_suspend;

template< typename Signature, typename D >
struct coroutine_base_suspend< Signature, D, void, 0 >
{
    void suspend()
    {
        D * dp = static_cast< D * >( this);
        dp->native_suspend( 0);
    }
};

template< typename Signature, typename D, int arity >
struct coroutine_base_suspend< Signature, D, void, arity >
{
    void suspend()
    {
        D * dp = static_cast< D * >( this);
        dp->native_suspend( 0);
    }
};

template< typename Signature, typename D, typename Result >
struct coroutine_base_suspend< Signature, D, Result, 0 >
{
    void suspend( typename param_type< Result >::type param_)
    {
        D * dp = static_cast< D * >( this);
        dp->result_ = typename param_type< Result >::type( param_);
        dp->native_suspend( 0);
    }
};

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_base_suspend
{
    void suspend( typename param_type< Result >::type param_)
    {
        D * dp = static_cast< D * >( this);
        dp->result_ = typename param_type< Result >::type( param_);
        dp->native_suspend( 0);
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_BASE_SUSPEND_H
