
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_CONTEXT_OBJECT_H
#define BOOST_CORO_DETAIL_CONTEXT_OBJECT_H

#include <cstddef>

#include <boost/config.hpp>
#include <boost/move/move.hpp>
#include <boost/ref.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/context_exec.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Fn, typename StackAllocator, typename Signature, typename Result, int arity >
class context_object :
    public context_exec<
        Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
        Result, arity
    >
{
private:
    template< typename X, typename Y, typename R, int >
    friend class context_exec;

    Fn              fn_;
    StackAllocator  alloc_;

    context_object( context_object &);
    context_object & operator=( context_object const&);

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    context_object( Fn && fn, attributes const& attr, StackAllocator const& alloc) :
        context_exec<
            Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
            Result, arity
        >( attr, alloc),
        fn_( static_cast< Fn && >( fn) ), alloc_( alloc)
    {}
#else
    context_object( Fn fn, attributes const& attr, StackAllocator const& alloc) :
        context_exec<
            Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
            Result, arity
        >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}

    context_object( BOOST_RV_REF( Fn) fn, attributes const& attr, StackAllocator const& alloc) :
        context_exec<
            Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
            Result, arity
        >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}
#endif

    ~context_object()
    { deallocate( alloc_); }
};

template< typename Fn, typename StackAllocator, typename Signature, typename Result, int arity >
class context_object< reference_wrapper< Fn >, StackAllocator, Signature, Result, arity > :
    public context_exec<
        Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
        Result, arity
    >
{
private:
    template< typename X, typename Y, typename R, int >
    friend class context_exec;

    Fn              fn_;
    StackAllocator  alloc_;

    context_object( context_object &);
    context_object & operator=( context_object const&);

public:
    context_object( reference_wrapper< Fn > fn, attributes const& attr, StackAllocator const& alloc) :
        context_exec<
            Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
            Result, arity
        >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}

    ~context_object()
    { deallocate( alloc_); }
};

template< typename Fn, typename StackAllocator, typename Signature, typename Result, int arity >
class context_object< const reference_wrapper< Fn >, StackAllocator, Signature, Result, arity > :
    public context_exec<
        Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
        Result, arity
    >
{
private:
    template< typename X, typename Y, typename R, int >
    friend class context_exec;

    Fn              fn_;
    StackAllocator  alloc_;

    context_object( context_object &);
    context_object & operator=( context_object const&);

public:
    context_object( const reference_wrapper< Fn > fn, attributes const& attr, StackAllocator const& alloc) :
        context_exec<
            Signature, context_object< Fn, StackAllocator, Signature, Result, arity >,
            Result, arity
        >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}

    ~context_object()
    { deallocate( alloc_); }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_CONTEXT_OBJECT_H
