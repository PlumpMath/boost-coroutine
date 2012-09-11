
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_GENERATOR_H
#define BOOST_CORO_GENERATOR_H

#include <cstddef>
#include <memory>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/stack_allocator.hpp>
#include <boost/context/stack_utils.hpp>
#include <boost/move/move.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/result_of.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/generator_base.hpp>
#include <boost/coroutine/detail/generator_object.hpp>
#include <boost/coroutine/detail/generator_resume.hpp>
#include <boost/coroutine/detail/generator_self.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {

template< typename Result >
class generator :
    public detail::generator_resume<
        Result, generator< Result >
    >
{
private:
    typedef detail::generator_base< Result >    base_t;
    typedef typename base_t::ptr_t              ptr_t;

    template< typename X, typename Y >
    friend class detail::generator_resume;

    ptr_t  impl_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( generator);

public:
    typedef detail::generator_self< Result >    self_t;
    typedef void ( * unspecified_bool_type)( generator ***);

    static void unspecified_bool( generator ***) {}

    generator() BOOST_NOEXCEPT :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {}

#ifndef BOOST_NO_RVALUE_REFERENCES
    template< typename Fn >
    generator( Fn && fn, attributes const& attr = attributes(),
               ctx::stack_allocator const& stack_alloc = ctx::stack_allocator(),
               std::allocator< generator > const& alloc = std::allocator< generator >() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                ctx::stack_allocator,
                std::allocator< generator >,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    generator( Fn && fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< generator > const& alloc = std::allocator< generator >() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                StackAllocator,
                std::allocator< generator >,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    generator( Fn && fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                StackAllocator,
                Allocator,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( static_cast< Fn && >( fn), attr, stack_alloc, a) );
    }
#else
    template< typename Fn >
    generator( Fn fn, attributes const& attr = attributes(),
               ctx::stack_allocator const& stack_alloc = ctx::stack_allocator(),
               std::allocator< generator > const& alloc = std::allocator< generator >() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                ctx::stack_allocator,
                std::allocator< generator >,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    generator( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< generator > const& alloc = std::allocator< generator >() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                StackAllocator,
                std::allocator< generator >,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    generator( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                StackAllocator,
                Allocator,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn >
    generator( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
            ctx::stack_allocator const& stack_alloc = ctx::stack_allocator(),
            std::allocator< generator > const& alloc = std::allocator< generator >() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                ctx::stack_allocator,
                std::allocator< generator >,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    generator( BOOST_RV_REF( Fn) fn, attributes const& attr,
            StackAllocator const& stack_alloc,
            std::allocator< generator > const& alloc = std::allocator< generator >() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                StackAllocator,
                std::allocator< generator >,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    generator( BOOST_RV_REF( Fn) fn, attributes const& attr,
            StackAllocator const& stack_alloc,
            Allocator const& alloc) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    {
        BOOST_MPL_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >));

        typedef detail::generator_object<
                Fn,
                StackAllocator,
                Allocator,
                Result
            >                               object_t;
        typename object_t::allocator_t a( alloc);

        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }
#endif

    generator( BOOST_RV_REF( generator) other) BOOST_NOEXCEPT :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_()
    { swap( other); }

    generator & operator=( BOOST_RV_REF( generator) other) BOOST_NOEXCEPT
    {
        if ( this == & other) return * this;
        generator tmp( boost::move( other) );
        swap( tmp);
        return * this;
    }

    operator unspecified_bool_type() const BOOST_NOEXCEPT
    { return impl_ ? unspecified_bool : 0; }

    bool operator!() const BOOST_NOEXCEPT
    { return ! impl_; }

    void swap( generator & other) BOOST_NOEXCEPT
    { impl_.swap( other.impl_); }
};

template< typename Result >
void swap( generator< Result > & l, generator< Result > & r) BOOST_NOEXCEPT
{ l.swap( r); }

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_GENERATOR_H
