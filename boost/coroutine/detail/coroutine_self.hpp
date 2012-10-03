
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_SELF_H
#define BOOST_CORO_DETAIL_COROUTINE_SELF_H

#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/int.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/param_type.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Allocator, typename Signature, typename Result, int arity >
class coroutine_self : public  coroutine_base< Signature, Result, arity >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_self<
            Allocator, Signature, Result, arity
        >
    >::other   allocator_t;

    coroutine_self( context::fcontext_t * callee, bool preserve_fpu,
                    allocator_t const& alloc) BOOST_NOEXCEPT :
        coroutine_base< Signature, Result, arity >( callee, preserve_fpu),
        alloc_( alloc)
    {}

    void deallocate_object()
    { destroy_( alloc_, this); }

private:
    allocator_t   alloc_;

    static void destroy_( allocator_t & alloc, coroutine_self * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_SELF_H
