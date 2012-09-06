
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

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/context_exec.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Fn, typename Signature, typename StackAllocator, typename Result, int arity >
class context_object :
    public context_exec<
        Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
        Result, arity
    >
{
private:
    template< typename X, typename Y, typename Z, typename R, int >
    friend class context_exec;

    Fn  fn_;

    context_object( context_object &);
    context_object & operator=( context_object const&);

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    context_object( Fn && fn, StackAllocator const& alloc,
                    std::size_t size, flag_unwind_t do_unwind, bool preserve_fpu) :
        context_exec<
            Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
            Result, arity
        >( alloc, size, do_unwind, preserve_fpu),
        fn_( static_cast< Fn && >( fn) )
    {}
#else
    context_object( Fn fn, StackAllocator const& alloc,
                    std::size_t size, flag_unwind_t do_unwind, bool preserve_fpu) :
        context_exec<
            Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
            Result, arity
        >( alloc, size, do_unwind, preserve_fpu),
        fn_( fn)
    {}

    context_object( BOOST_RV_REF( Fn) fn, StackAllocator const& alloc,
                    std::size_t size, flag_unwind_t do_unwind, bool preserve_fpu) :
        context_exec<
            Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
            Result, arity
        >( alloc, size, do_unwind, preserve_fpu),
        fn_( fn)
    {}
#endif
};

template< typename Fn, typename Signature, typename StackAllocator, typename Result, int arity >
class context_object< reference_wrapper< Fn >, Signature, StackAllocator, Result, arity > :
    public context_exec<
        Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
        Result, arity
    >
{
private:
    template< typename X, typename Y, typename Z, typename R, int >
    friend class context_exec;

    Fn      fn_;

    context_object( context_object &);
    context_object & operator=( context_object const&);

public:
    context_object( reference_wrapper< Fn > fn, StackAllocator const& alloc,
                    std::size_t size, flag_unwind_t do_unwind, bool preserve_fpu) :
        context_exec<
            Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
            Result, arity
        >( alloc, size, do_unwind, preserve_fpu),
        fn_( fn)
    {}
};

template< typename Fn, typename Signature, typename StackAllocator, typename Result, int arity >
class context_object< const reference_wrapper< Fn >, Signature, StackAllocator, Result, arity > :
    public context_exec<
        Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
        Result, arity
    >
{
private:
    template< typename X, typename Y, typename Z, typename R, int >
    friend class context_exec;

    Fn      fn_;

    context_object( context_object &);
    context_object & operator=( context_object const&);

public:
    context_object( const reference_wrapper< Fn > fn, StackAllocator const& alloc,
            std::size_t size, flag_unwind_t do_unwind, bool preserve_fpu) :
        context_exec<
            Signature, StackAllocator, context_object< Fn, Signature, StackAllocator, Result, arity >,
            Result, arity
        >( alloc, size, do_unwind, preserve_fpu),
        fn_( fn)
    {}
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_CONTEXT_OBJECT_H
