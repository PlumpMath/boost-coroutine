
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_BASE_SUSPEND_H
#define BOOST_CORO_DETAIL_COROUTINE_BASE_SUSPEND_H

#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/param_type.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

struct forced_unwind {};

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_base_suspend;

template< typename Signature, typename D, int arity >
struct coroutine_base_suspend< Signature, D, void, arity >
{
    void suspend()
    {
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() );

        context::jump_fcontext(
            static_cast< D * >( this)->callee_,
            & static_cast< D * >( this)->caller_,
            0, fpu_preserved == static_cast< D * >( this)->preserve_fpu_);
        if ( static_cast< D * >( this)->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
    }
};

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_base_suspend
{
    typedef typename param_type< Result >::type param_t;

    void suspend( param_t param)
    {
        static_cast< D * >( this)->result_ = param;
        context::jump_fcontext(
            static_cast< D * >( this)->callee_,
            & static_cast< D * >( this)->caller_,
            0, fpu_preserved == static_cast< D * >( this)->preserve_fpu_);
        if ( static_cast< D * >( this)->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_BASE_SUSPEND_H
