
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_GENERATOR_H
#define BOOST_CORO_GENERATOR_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/stack_allocator.hpp>
#include <boost/context/stack_utils.hpp>
#include <boost/move/move.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/context_base.hpp>
#include <boost/coroutine/detail/context_object.hpp>
#include <boost/coroutine/detail/generator_resume.hpp>
#include <boost/coroutine/detail/context_self.hpp>
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
    typedef detail::context_base<
        Result(), Result, 0
     >                                      base_t;
    typedef typename base_t::ptr_t          ptr_t;

    template< typename X, typename Y >
    friend class detail::generator_resume;

    ptr_t  impl_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( generator);

public:
    typedef detail::context_self<
        Result(), Result, 0
    >                                       self_t;
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
               ctx::stack_allocator const& stack_alloc = ctx::stack_allocator() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_(
            new detail::context_object<
                Fn, ctx::stack_allocator, Result(), Result, 0
            >( static_cast< Fn && >( fn), attr, stack_alloc) )
    { this->fetch_(); }

    template< typename Fn, typename StackAllocator >
    generator( Fn && fn, attributes const& attr = attributes(),
               StackAllocator const& stack_alloc = StackAllocator() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_(
            new detail::context_object<
                Fn, StackAllocator, Result(), Result, 0
            >( static_cast< Fn && >( fn), attr, stack_alloc) )
    { this->fetch_(); }
#else
    template< typename Fn >
    generator( Fn fn, attributes const& attr = attributes(),
            ctx::stack_allocator const& stack_alloc = ctx::stack_allocator() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_(
            new detail::context_object<
                Fn, ctx::stack_allocator, Result(), Result, 0
            >( fn, attr, stack_alloc) )
    { this->fetch_(); }

    template< typename Fn, typename StackAllocator >
    generator( Fn fn, attributes const& attr = attributes(),
            StackAllocator const& stack_alloc = StackAllocator() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_(
            new detail::context_object<
                Fn, StackAllocator, Result(), Result, 0
            >( fn, attr, stack_alloc) )
    { this->fetch_(); }

    template< typename Fn >
    generator( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
            ctx::stack_allocator const& stack_alloc = ctx::stack_allocator() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_(
            new detail::context_object<
                Fn, ctx::stack_allocator, Result(), Result, 0
            >( fn, attr, stack_alloc) )
    { this->fetch_(); }

    template< typename Fn, typename StackAllocator >
    generator( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
            StackAllocator const& stack_alloc = StackAllocator() ) :
        detail::generator_resume<
            Result, generator< Result >
        >(),
        impl_(
            new detail::context_object<
                Fn, StackAllocator, Result(), Result, 0
            >( fn, attr, stack_alloc) )
    { this->fetch_(); }
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
    { return impl_ && this->has_value_() ? unspecified_bool : 0; }

    bool operator!() const BOOST_NOEXCEPT
    { return ! impl_ || ! this->has_value_(); }

    void swap( generator & other) BOOST_NOEXCEPT
    {
        impl_.swap( other.impl_);
        this->swap_( other);
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
