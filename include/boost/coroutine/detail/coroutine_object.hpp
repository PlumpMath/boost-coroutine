
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_DETAIL_COROUTINE_OBJECT_H
#define BOOST_COROUTINES_DETAIL_COROUTINE_OBJECT_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/move/move.hpp>
#include <boost/ref.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/holder.hpp>
#include <boost/coroutine/detail/param.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

template< typename Coroutine >
void trampoline1( intptr_t vp)
{
    BOOST_ASSERT( vp);

    reinterpret_cast< Coroutine * >( vp)->run();
}

template< typename Coroutine, typename Arg >
void trampoline2( intptr_t vp)
{
    BOOST_ASSERT( vp);

    tuple< Coroutine *, Arg > * tpl(
        reinterpret_cast< tuple< Coroutine *, Arg > * >( vp) );
    Coroutine * coro( get< 0 >( * tpl) );
    Arg arg( get< 1 >( * tpl) );

    coro->run( arg);
}

#if defined(BOOST_USE_SEGMENTED_STACKS)
template< typename StackAllocator >
struct stack_data
{
    typedef void *  segmented_stack_context[BOOST_COROUTINES_SEGMENTS];

    StackAllocator          stack_alloc;
    void                *   sp;
    std::size_t             size;
    segmented_stack_context seg_stack;

    stack_data( StackAllocator const& stack_alloc_, std::size_t min_size) :
        stack_alloc( stack_alloc_),
        sp ( 0),
        size( 0)
    {
        sp = stack_alloc.allocate( min_size, & seg[0], & size);
    }

    ~stack_data()
    {
        stack_alloc.deallocate( & seg[0]);
    }
};
#else
template< typename StackAllocator >
struct stack_data
{
    StackAllocator  stack_alloc;
    void        *   sp;
    std::size_t     size;

    stack_data( StackAllocator const& stack_alloc_, std::size_t size_) :
        stack_alloc( stack_alloc_),
        sp ( 0),
        size( size_)
    { sp = stack_alloc.allocate( size); }

    ~stack_data()
    { stack_alloc.deallocate( sp, size); }
};
#endif

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller,
    typename Result, int arity
>
class coroutine_object;

#include <boost/coroutine/detail/coroutine_object_void_0.ipp>
#include <boost/coroutine/detail/coroutine_object_void_1.ipp>
#include <boost/coroutine/detail/coroutine_object_void_arity.ipp>
#include <boost/coroutine/detail/coroutine_object_result_0.ipp>
#include <boost/coroutine/detail/coroutine_object_result_1.ipp>
#include <boost/coroutine/detail/coroutine_object_result_arity.ipp>

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_DETAIL_COROUTINE_OBJECT_H
