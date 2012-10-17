
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_COROUTINE_H
#define BOOST_CORO_COROUTINE_H

#include <cstddef>
#include <memory>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/context/guarded_stack_allocator.hpp>
#include <boost/cstdint.hpp>
#include <boost/move/move.hpp>
#include <boost/range.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/result_of.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/coroutine_get.hpp>
#include <boost/coroutine/detail/coroutine_object.hpp>
#include <boost/coroutine/detail/coroutine_op.hpp>
#include <boost/coroutine/detail/coroutine_caller.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< template< class, int > class C, typename Signature, typename Result, int arity >
struct caller;

template< template< class, int > class C, typename Signature >
struct caller< C, Signature, void, 0 >
{ typedef C< void(), 0 >   type; };

template< template< class, int > class C, typename Signature, typename Result >
struct caller< C, Signature, Result, 0 >
{ typedef C< void( Result), 1 >   type; };

template< template< class, int > class C, typename Signature, int arity >
struct caller< C, Signature, void, arity >
{ typedef C< typename detail::arg< Signature >::type_t(), 0 >   type; };

template< template< class, int > class C, typename Signature, typename Result, int arity >
struct caller
{ typedef C< typename detail::arg< Signature >::type_t( Result), 1 >   type; };

}

template< typename Signature, int arity = function_traits< Signature >::arity >
class coroutine;

template< typename Signature >
class coroutine< Signature, 0 > : public detail::coroutine_op<
                        Signature, coroutine< Signature >,
                        typename function_traits< Signature >::result_type,
                        0
                  >,
                  public detail::coroutine_get<
                        Signature, coroutine< Signature >,
                        typename function_traits< Signature >::result_type,
                        0
                  >
{
private:
    typedef detail::coroutine_base<
        Signature,
        typename function_traits< Signature >::result_type,
        0
     >                                                          base_t;
    typedef typename base_t::ptr_t                              ptr_t;

    template< typename X, typename Y, typename Z, int >
    friend struct detail::coroutine_get;
    template< typename X, typename Y, typename Z, int >
    friend struct detail::coroutine_op;
    template< typename X, typename Y, typename Z, typename A, typename B, int, typename C >
    friend class detail::coroutine_object;

    struct dummy
    { void nonnull() {} };

    typedef void ( dummy::*safe_bool)();

    ptr_t  impl_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( coroutine);

public:
    typedef typename detail::caller<
        coroutine,
        Signature,
        typename function_traits< Signature >::result_type,
        0
    >::type                                                     caller_t;

    coroutine() BOOST_NOEXCEPT :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {}

    template< typename Allocator >
    coroutine( context::fcontext_t * callee,
               bool preserve_fpu,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        typedef detail::coroutine_caller<
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                0
            >                               self_t;
        typename self_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) self_t(
                callee, preserve_fpu, a) );
    }

#ifndef BOOST_NO_RVALUE_REFERENCES
    template< typename Fn >
    coroutine( Fn && fn, attributes const& attr = attributes(),
            context::guarded_stack_allocator const& stack_alloc =
                context::guarded_stack_allocator(),
            std::allocator< coroutine > const& alloc =
                std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    coroutine( Fn && fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    coroutine( Fn && fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }
#else
    template< typename Fn >
    coroutine( Fn fn, attributes const& attr = attributes(),
               context::guarded_stack_allocator const& stack_alloc =
                    context::guarded_stack_allocator(),
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    coroutine( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    coroutine( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
               context::guarded_stack_allocator const& stack_alloc =
                    context::guarded_stack_allocator(),
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                0,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }
#endif

    coroutine( BOOST_RV_REF( coroutine) other) BOOST_NOEXCEPT :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            0
        >(),
        impl_()
    { swap( other); }

    coroutine & operator=( BOOST_RV_REF( coroutine) other) BOOST_NOEXCEPT
    {
        coroutine tmp( boost::move( other) );
        swap( tmp);
        return * this;
    }

    bool empty() const
    { return ! impl_; }

    operator safe_bool() const BOOST_NOEXCEPT
    { return ( empty() || impl_->is_complete() ) ? 0 : & dummy::nonnull; }

    bool operator!() const BOOST_NOEXCEPT
    { return empty() || impl_->is_complete(); }

    void swap( coroutine & other) BOOST_NOEXCEPT
    { impl_.swap( other.impl_); }
};

template< typename Signature, int arity >
class coroutine : public detail::coroutine_op<
                        Signature, coroutine< Signature >,
                        typename function_traits< Signature >::result_type,
                        function_traits< Signature >::arity
                  >,
                  public detail::coroutine_get<
                        Signature, coroutine< Signature >,
                        typename function_traits< Signature >::result_type,
                        function_traits< Signature >::arity
                  >
{
private:
    typedef detail::coroutine_base<
        Signature,
        typename function_traits< Signature >::result_type,
        function_traits< Signature >::arity
     >                                                          base_t;
    typedef typename base_t::ptr_t                              ptr_t;

    template< typename X, typename Y, typename Z, int >
    friend struct detail::coroutine_get;
    template< typename X, typename Y, typename Z, int >
    friend struct detail::coroutine_op;
    template< typename X, typename Y, typename Z, typename A, typename B, int, typename C >
    friend class detail::coroutine_object;

    struct dummy
    { void nonnull() {} };

    typedef void ( dummy::*safe_bool)();

    ptr_t  impl_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( coroutine);

public:
    typedef typename detail::caller<
        coroutine,
        Signature,
        typename function_traits< Signature >::result_type,
        function_traits< Signature >::arity
    >::type                                                     caller_t;
    typedef typename detail::arg< Signature >::type_t           arg_type;

    coroutine() BOOST_NOEXCEPT :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {}

    template< typename Allocator >
    coroutine( context::fcontext_t * callee,
               bool preserve_fpu,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        typedef detail::coroutine_caller<
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >                               self_t;
        typename self_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) self_t(
                callee, preserve_fpu, a) );
    }

#ifndef BOOST_NO_RVALUE_REFERENCES
    template< typename Fn >
    coroutine( Fn && fn, attributes const& attr = attributes(),
            context::guarded_stack_allocator const& stack_alloc =
                context::guarded_stack_allocator(),
            std::allocator< coroutine > const& alloc =
                std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn >
    coroutine( Fn && fn, arg_type arg, attributes const& attr = attributes(),
            context::guarded_stack_allocator const& stack_alloc =
                context::guarded_stack_allocator(),
            std::allocator< coroutine > const& alloc =
                std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), arg, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    coroutine( Fn && fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    coroutine( Fn && fn, arg_type arg, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), arg, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    coroutine( Fn && fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    coroutine( Fn && fn, arg_type arg, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t(
                static_cast< Fn && >( fn), arg, attr, stack_alloc, a) );
    }
#else
    template< typename Fn >
    coroutine( Fn fn, attributes const& attr = attributes(),
               context::guarded_stack_allocator const& stack_alloc =
                    context::guarded_stack_allocator(),
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn >
    coroutine( Fn fn, arg_type arg, attributes const& attr = attributes(),
               context::guarded_stack_allocator const& stack_alloc =
                    context::guarded_stack_allocator(),
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, arg, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    coroutine( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    coroutine( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
               context::guarded_stack_allocator const& stack_alloc =
                    context::guarded_stack_allocator(),
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                context::guarded_stack_allocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< coroutine > const& alloc =
                    std::allocator< coroutine >() ) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                std::allocator< coroutine >,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {
        BOOST_STATIC_ASSERT((
            is_same< void, typename result_of< Fn() >::type >::value));
        typedef detail::coroutine_object<
                Fn,
                StackAllocator,
                Allocator,
                Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity,
                caller_t
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }
#endif

    coroutine( BOOST_RV_REF( coroutine) other) BOOST_NOEXCEPT :
        detail::coroutine_op<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        detail::coroutine_get<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    { swap( other); }

    coroutine & operator=( BOOST_RV_REF( coroutine) other) BOOST_NOEXCEPT
    {
        coroutine tmp( boost::move( other) );
        swap( tmp);
        return * this;
    }

    bool empty() const
    { return ! impl_; }

    operator safe_bool() const BOOST_NOEXCEPT
    { return ( empty() || impl_->is_complete() ) ? 0 : & dummy::nonnull; }

    bool operator!() const BOOST_NOEXCEPT
    { return empty() || impl_->is_complete(); }

    void swap( coroutine & other) BOOST_NOEXCEPT
    { impl_.swap( other.impl_); }
};

template< typename Signature >
void swap( coroutine< Signature > & l, coroutine< Signature > & r) BOOST_NOEXCEPT
{ l.swap( r); }

template< typename Signature >
inline
typename coroutine< Signature >::iterator
range_begin( coroutine< Signature > & c)
{ return typename coroutine< Signature >::iterator( & c); }

template< typename Signature >
inline
typename coroutine< Signature >::const_iterator
range_begin( coroutine< Signature > const& c)
{ return typename coroutine< Signature >::const_iterator( & c); }

template< typename Signature >
inline
typename coroutine< Signature >::iterator
range_end( coroutine< Signature > & c)
{ return typename coroutine< Signature >::iterator(); }

template< typename Signature >
inline
typename coroutine< Signature >::const_iterator
range_end( coroutine< Signature > const& c)
{ return typename coroutine< Signature >::const_iterator(); }

}

template< typename Signature >
struct range_mutable_iterator< coro::coroutine< Signature > >
{ typedef typename coro::coroutine< Signature >::iterator type; };

template< typename Signature >
struct range_const_iterator< coro::coroutine< Signature > >
{ typedef typename coro::coroutine< Signature >::const_iterator type; };

}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_COROUTINE_H
