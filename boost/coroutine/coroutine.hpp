
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
#include <boost/context/stack_allocator.hpp>
#include <boost/context/stack_utils.hpp>
#include <boost/cstdint.hpp>
#include <boost/move/move.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/context_base.hpp>
#include <boost/coroutine/detail/context_object.hpp>
#include <boost/coroutine/detail/coroutine_resume.hpp>
#include <boost/coroutine/detail/context_self.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {

template< typename Signature, typename Allocator = std::allocator< int > >
class coroutine :
    public detail::coroutine_resume<
        Signature, coroutine< Signature >,
        typename function_traits< Signature >::result_type,
        function_traits< Signature >::arity
    >
{
private:
    typedef detail::context_base<
        Signature, 
        typename function_traits< Signature >::result_type,
        function_traits< Signature >::arity
     >                                                          base_t;
    typedef typename base_t::ptr_t                              ptr_t;

    template< typename X, typename Y, typename Z, int >
    friend struct detail::coroutine_resume;

    ptr_t  impl_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( coroutine);

public:
    typedef detail::context_self<
        Signature,
        typename function_traits< Signature >::result_type,
        function_traits< Signature >::arity
    >                                                           self_t;
    typedef void ( * unspecified_bool_type)( coroutine ***);

    static void unspecified_bool( coroutine ***) {}

    coroutine() BOOST_NOEXCEPT :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {}

#ifndef BOOST_NO_RVALUE_REFERENCES
    template< typename Fn >
    coroutine( Fn && fn, attributes const& attr = attributes(),
               ctx::stack_allocator const& stack_alloc = ctx::stack_allocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, ctx::stack_allocator, Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( static_cast< Fn && >( fn), attr, stack_alloc) )
    {}

    template< typename Fn, typename StackAllocator >
    coroutine( Fn && fn, attributes const& attr = attributes(),
               StackAllocator const& stack_alloc = StackAllocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, StackAllocator, Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( static_cast< Fn && >( fn), attr, stack_alloc) )
    {}
#else
    template< typename Fn >
    coroutine( Fn fn, attributes const& attr = attributes(),
               ctx::stack_allocator const& stack_alloc = ctx::stack_allocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
           new detail::context_object<
               Fn, ctx::stack_allocator, Signature,
               typename function_traits< Signature >::result_type,
               function_traits< Signature >::arity
           >( fn, attr, stack_alloc) )
    {
//        typedef detail::context_object<
//                Fn,
//                ctx::stack_allocator,
//                Signature,
//                typename function_traits< Signature >::result_type,
//                function_traits< Signature >::arity,
//                Allocator
//            >                               context_t;
//        typename context_t::allocator_t a;
//        context_t * p = a.allocate( 1); // must be destroyed in  the case of an exception
//        a.construct( p, context_t( fn, attr, stack_alloc, a) );
//        impl_ = p;
    }

    template< typename Fn, typename StackAllocator >
    coroutine( Fn fn, attributes const& attr = attributes(),
               StackAllocator const& stack_alloc = StackAllocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, StackAllocator, Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( fn, attr, stack_alloc) )
    {}

    template< typename Fn >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
               ctx::stack_allocator const& stack_alloc = ctx::stack_allocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, ctx::stack_allocator, Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( fn, attr, stack_alloc) )
    {}

    template< typename Fn, typename StackAllocator >
    coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
               StackAllocator const& stack_alloc = StackAllocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, StackAllocator, Signature,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( fn, attr, stack_alloc) )
    {}
#endif

    coroutine( BOOST_RV_REF( coroutine) other) BOOST_NOEXCEPT :
        detail::coroutine_resume<
            Signature, coroutine< Signature >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    { swap( other); }

    coroutine & operator=( BOOST_RV_REF( coroutine) other) BOOST_NOEXCEPT
    {
        if ( this == & other) return * this;
        coroutine tmp( boost::move( other) );
        swap( tmp);
        return * this;
    }

    operator unspecified_bool_type() const BOOST_NOEXCEPT
    { return impl_ ? unspecified_bool : 0; }

    bool operator!() const BOOST_NOEXCEPT
    { return ! impl_; }

    void swap( coroutine & other) BOOST_NOEXCEPT
    { impl_.swap( other.impl_); }

    bool is_complete() const BOOST_NOEXCEPT
    {
        BOOST_ASSERT( impl_);
        return impl_->is_complete();
    }
};

template< typename Signature >
void swap( coroutine< Signature > & l, coroutine< Signature > & r) BOOST_NOEXCEPT
{ l.swap( r); }

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_COROUTINE_H
