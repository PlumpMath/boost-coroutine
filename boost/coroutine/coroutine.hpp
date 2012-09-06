
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_COROUTINE_H
#define BOOST_CORO_COROUTINE_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/stack_allocator.hpp>
#include <boost/context/stack_utils.hpp>
#include <boost/cstdint.hpp>
#include <boost/move/move.hpp>
#include <boost/type_traits/function_traits.hpp>

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

template<
    typename Signature,
    typename StackAllocator = ctx::stack_allocator
>
class coroutine :
    public detail::coroutine_resume<
        Signature, coroutine< Signature, StackAllocator >,
        typename function_traits< Signature >::result_type,
        function_traits< Signature >::arity
    >
{
private:
    typedef detail::context_base<
        Signature, StackAllocator,
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
        Signature, StackAllocator,
        typename function_traits< Signature >::result_type,
        function_traits< Signature >::arity
    >                                                           self_t;
    typedef void ( * unspecified_bool_type)( coroutine ***);

    static void unspecified_bool( coroutine ***) {}

    coroutine() BOOST_NOEXCEPT :
        detail::coroutine_resume<
            Signature, coroutine< Signature, StackAllocator >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_()
    {}

#ifndef BOOST_NO_RVALUE_REFERENCES
    template< typename Fn >
    coroutine( Fn && fn, std::size_t size = ctx::default_stacksize(),
               flag_unwind_t do_unwind = stack_unwind,
               bool preserve_fpu = true,
               StackAllocator const& alloc = StackAllocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature, StackAllocator >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, Signature, StackAllocator,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( static_cast< Fn && >( fn), alloc, size, do_unwind, preserve_fpu) )
    {}
#else
    template< typename Fn >
    coroutine( Fn fn, std::size_t size = ctx::default_stacksize(),
               flag_unwind_t do_unwind = stack_unwind,
               bool preserve_fpu = true,
               StackAllocator const& alloc = StackAllocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature, StackAllocator >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, Signature, StackAllocator,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( fn, alloc, size, do_unwind, preserve_fpu) )
    {}

    template< typename Fn >
    coroutine( BOOST_RV_REF( Fn) fn, std::size_t size = ctx::default_stacksize(),
               flag_unwind_t do_unwind = stack_unwind,
               bool preserve_fpu = true,
               StackAllocator const& alloc = StackAllocator() ) :
        detail::coroutine_resume<
            Signature, coroutine< Signature, StackAllocator >,
            typename function_traits< Signature >::result_type,
            function_traits< Signature >::arity
        >(),
        impl_(
            new detail::context_object<
                Fn, Signature, StackAllocator,
                typename function_traits< Signature >::result_type,
                function_traits< Signature >::arity
            >( fn, alloc, size, do_unwind, preserve_fpu) )
    {}
#endif

    coroutine( BOOST_RV_REF( coroutine) other) BOOST_NOEXCEPT :
        detail::coroutine_resume<
            Signature, coroutine< Signature, StackAllocator >,
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

template<
    typename Signature,
    typename StackAllocator
>
void swap( coroutine< Signature, StackAllocator > & l, coroutine< Signature, StackAllocator > & r) BOOST_NOEXCEPT
{ l.swap( r); }

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_COROUTINE_H
