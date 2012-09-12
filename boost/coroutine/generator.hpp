
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
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/utility/result_of.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/generator_base.hpp>
#include <boost/coroutine/detail/generator_object.hpp>
#include <boost/coroutine/detail/generator_self.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {

template< typename Result >
class generator
{
private:
    typedef detail::generator_base< Result >    base_t;
    typedef typename base_t::ptr_t              ptr_t;

    ptr_t  impl_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( generator);

public:
    typedef detail::generator_self< Result >    self_t;
    typedef void ( * unspecified_bool_type)( generator ***);

    static void unspecified_bool( generator ***) {}

    generator() BOOST_NOEXCEPT :
        impl_()
    {}

#ifndef BOOST_NO_RVALUE_REFERENCES
    template< typename Fn >
    generator( Fn && fn, attributes const& attr = attributes(),
               ctx::stack_allocator const& stack_alloc = ctx::stack_allocator(),
               std::allocator< generator > const& alloc = std::allocator< generator >() ) :
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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
        impl_()
    {
        BOOST_STATIC_ASSERT((! is_same< void, Result >::value));
        BOOST_STATIC_ASSERT((
            is_same<
                void,
                typename result_of< Fn() >::type
            >::value));

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

    boost::optional< Result > operator()()
    {
        optional< Result >  result;
        try
        {
            if ( ! impl_->is_complete() )
            {
                if ( ! impl_->is_started() ) impl_->start( result);
                else impl_->resume( result);
            }
            else result = none;
        }
        catch ( coroutine_terminated const&)
        { result = none; }
        return result;
    }
};

template< typename Result >
void swap( generator< Result > & l, generator< Result > & r) BOOST_NOEXCEPT
{ l.swap( r); }

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_GENERATOR_H
