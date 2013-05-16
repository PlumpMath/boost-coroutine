
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_V2_DETAIL_PULL_COROUTINE_OBJECT_H
#define BOOST_COROUTINES_V2_DETAIL_PULL_COROUTINE_OBJECT_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/move/move.hpp>
#include <boost/ref.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/holder.hpp>
#include <boost/coroutine/detail/param.hpp>
#include <boost/coroutine/detail/stack_tuple.hpp>
#include <boost/coroutine/detail/trampoline.hpp>
#include <boost/coroutine/flags.hpp>
#include <boost/coroutine/stack_context.hpp>
#include <boost/coroutine/v2/detail/pull_coroutine_base.hpp>

#ifdef BOOST_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4355) // using 'this' in initializer list
#endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {
namespace detail {

template<
    typename Arg, typename Fn,
    typename StackAllocator, typename Allocator
>
class pull_coroutine_object< Arg, Fn, StackAllocator, Allocator > :
    private stack_tuple< StackAllocator >,
    public pull_coroutine_base< Arg >
{
public:
    typedef typename Allocator::template rebind<
        pull_coroutine_object<
            Arg, Fn, StackAllocator, Allocator
        >
    >::other                                            allocator_t;

private:
    typedef stack_tuple< StackAllocator >               pbase_type;
    typedef pull_coroutine_base< Arg >                  base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, pull_coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    pull_coroutine_object( pull_coroutine_object &);
    pull_coroutine_object & operator=( pull_coroutine_object const&);

    void enter_()
    {
        holder< R > * hldr_from(
            reinterpret_cast< holder< R > * >(
                caller_.jump(
                    callee_,
                    reinterpret_cast< intptr_t >( this),
                    preserve_fpu() ) ) );
        callee_ = * hldr_from->ctx;
        result_ = hldr_from->data;
        if ( except_) rethrow_exception( except_);
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! is_complete() );

        flags_ |= flag_unwind_stack;
        holder< Arg > hldr_to( & caller_, true);
        caller_.jump(
            callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            preserve_fpu() );
        flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( is_complete() );
    }

public:
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    pull_coroutine_object( Fn && fn, attributes const& attr,
                           StackAllocator const& stack_alloc,
                           allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< pull_coroutine_object >,
            & stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( forward< Fn >( fn) ),
        alloc_( alloc)
    { enter_(); }
#else
    pull_coroutine_object( Fn fn, attributes const& attr,
                           StackAllocator const& stack_alloc,
                           allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< pull_coroutine_object >,
            & stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    pull_coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< pull_coroutine_object >,
            & stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }
#endif

    ~pull_coroutine_object()
    {
        if ( ! is_complete() && force_unwind() )
            unwind_stack_();
    }

    void run()
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            // create push_coroutine
            Caller c( * hldr_from->ctx, false, preserve_fpu(), alloc_);
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        flags_ |= flag_complete;
        holder< R > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            preserve_fpu() );
        BOOST_ASSERT_MSG( false, "pull_coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Arg, typename Fn,
    typename StackAllocator, typename Allocator
>
class pull_coroutine_object< Arg, reference_wrapper< Fn >, StackAllocator, Allocator > :
    private stack_tuple< StackAllocator >,
    public pull_coroutine_base< Arg >
{
public:
    typedef typename Allocator::template rebind<
        pull_coroutine_object<
            Arg, Fn, StackAllocator, Allocator
        >
    >::other                                            allocator_t;

private:
    typedef stack_tuple< StackAllocator >               pbase_type;
    typedef pull_coroutine_base< Arg >                  base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, pull_coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    pull_coroutine_object( pull_coroutine_object &);
    pull_coroutine_object & operator=( pull_coroutine_object const&);

    void enter_()
    {
        holder< R > * hldr_from(
            reinterpret_cast< holder< R > * >(
                caller_.jump(
                    callee_,
                    reinterpret_cast< intptr_t >( this),
                    preserve_fpu() ) ) );
        callee_ = * hldr_from->ctx;
        if ( except_) rethrow_exception( except_);
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! is_complete() );

        flags_ |= flag_unwind_stack;
        holder< Arg > hldr_to( & caller_, true);
        caller_.jump(
            callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            preserve_fpu() );
        flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( is_complete() );
    }

public:
    pull_coroutine_object( reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< pull_coroutine_object >,
            & stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    ~pull_coroutine_object()
    {
        if ( ! is_complete() && force_unwind() )
            unwind_stack_();
    }

    void run()
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            // create pull_coroutine
            Caller c( * hldr_from->ctx, false, preserve_fpu(), alloc_);
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        flags_ |= flag_complete;
        holder< R > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            preserve_fpu() );
        BOOST_ASSERT_MSG( false, "pull_coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Arg, typename Fn,
    typename StackAllocator, typename Allocator
>
class pull_coroutine_object< Arg, const reference_wrapper< Fn >, StackAllocator, Allocator > :
    private stack_tuple< StackAllocator >,
    public pull_coroutine_base< Arg >
{
public:
    typedef typename Allocator::template rebind<
        pull_coroutine_object<
            Arg, Fn, StackAllocator, Allocator
        >
    >::other                                            allocator_t;

private:
    typedef stack_tuple< StackAllocator >               pbase_type;
    typedef pull_coroutine_base< Arg >                  base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, pull_coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    pull_coroutine_object( pull_coroutine_object &);
    pull_coroutine_object & operator=( pull_coroutine_object const&);

    void enter_()
    {
        holder< R > * hldr_from(
            reinterpret_cast< holder< R > * >(
                caller_.jump(
                    callee_,
                    reinterpret_cast< intptr_t >( this),
                    preserve_fpu() ) ) );
        callee_ = * hldr_from->ctx;
        if ( except_) rethrow_exception( except_);
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! is_complete() );

        flags_ |= flag_unwind_stack;
        holder< Arg > hldr_to( & caller_, true);
        caller_.jump(
            callee_,
            reinterpret_cast< intptr_t >( & hldr_to),
            preserve_fpu() );
        flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( is_complete() );
    }

public:
    pull_coroutine_object( const reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< pull_coroutine_object >,
            & stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    ~pull_coroutine_object()
    {
        if ( ! is_complete() && force_unwind() )
            unwind_stack_();
    }

    void run()
    {
        coroutine_context callee;
        coroutine_context caller;

        {
            // create pull_coroutine
            Caller c( * hldr_from->ctx, false, preserve_fpu(), alloc_);
            try
            { fn_( c); }
            catch ( forced_unwind const&)
            {}
            catch (...)
            { except_ = current_exception(); }
            callee = c.impl_->callee_;
        }

        flags_ |= flag_complete;
        holder< R > hldr_to( & caller);
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & hldr_to),
            preserve_fpu() );
        BOOST_ASSERT_MSG( false, "pull_coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#ifdef BOOST_MSVC
 #pragma warning (pop)
#endif

#endif // BOOST_COROUTINES_V2_DETAIL_PULL_COROUTINE_OBJECT_H
