
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_CALLER_H
#define BOOST_CORO_DETAIL_COROUTINE_CALLER_H

#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Allocator, typename Signature, typename Result, int arity >
class coroutine_caller : public  coroutine_base< Signature, Result, arity >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_caller<
            Allocator, Signature, Result, arity
        >
    >::other   allocator_t;

    coroutine_caller( context::fcontext_t * callee, bool unwind, bool preserve_fpu,
                    allocator_t const& alloc) BOOST_NOEXCEPT :
        coroutine_base< Signature, Result, arity >( callee, unwind, preserve_fpu),
        alloc_( alloc)
    {}

    void deallocate_object()
    { destroy_( alloc_, this); }

private:
    allocator_t   alloc_;

    static void destroy_( allocator_t & alloc, coroutine_caller * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_CALLER_H
