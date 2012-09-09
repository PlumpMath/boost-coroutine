
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_OBJECT_H
#define BOOST_CORO_DETAIL_COROUTINE_OBJECT_H

#include <cstddef>

#include <boost/config.hpp>
#include <boost/move/move.hpp>
#include <boost/ref.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_exec.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Result, int arity
>
class coroutine_object :
    public coroutine_exec<
        Signature,
        coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
        Result,
        arity
    >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, Result, arity
        >
    >::other   allocator_t;

#ifndef BOOST_NO_RVALUE_REFERENCES
    coroutine_object( Fn && fn, attributes const& attr,
                    StackAllocator const& stack_alloc, allocator_t const& alloc) :
        coroutine_exec<
            Signature,
            coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
            Result,
            arity
        >( attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}
#else
    coroutine_object( Fn fn, attributes const& attr,
                    StackAllocator const& stack_alloc, allocator_t const& alloc) :
        coroutine_exec<
            Signature,
            coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
            Result,
            arity
        >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                    StackAllocator const& stack_alloc, allocator_t const& alloc) :
        coroutine_exec<
            Signature,
            coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
            Result,
            arity
        >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}
#endif

    ~coroutine_object()
    { deallocate_stack( stack_alloc_); }

    void deallocate_object()
    { destroy( alloc_, this); }

private:
    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    static void destroy( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Result, int arity
>
class coroutine_object< reference_wrapper< Fn >, StackAllocator, Allocator, Signature, Result, arity > :
    public coroutine_exec<
        Signature,
        coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
        Result,
        arity
    >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, Result, arity
        >
    >::other   allocator_t;

    coroutine_object( reference_wrapper< Fn > fn, attributes const& attr,
                    StackAllocator const& stack_alloc, allocator_t const& alloc) :
        coroutine_exec<
            Signature,
            coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
            Result,
            arity
        >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}

    ~coroutine_object()
    { deallocate_stack( stack_alloc_); }

    void deallocate_object()
    { destroy( alloc_, this); }

private:
    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    static void destroy( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Result, int arity
>
class coroutine_object< const reference_wrapper< Fn >, StackAllocator, Allocator, Signature, Result, arity > :
    public coroutine_exec<
        Signature,
        coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
        Result,
        arity
    >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, Result, arity
        >
    >::other   allocator_t;

    coroutine_object( const reference_wrapper< Fn > fn, attributes const& attr,
                    StackAllocator const& stack_alloc, allocator_t const& alloc) :
        coroutine_exec<
            Signature,
            coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, arity >,
            Result,
            arity
        >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}

    ~coroutine_object()
    { deallocate_stack( stack_alloc_); }

    void deallocate_object()
    { destroy( alloc_, this); }

private:
    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    static void destroy( allocator_t const& alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_OBJECT_H
