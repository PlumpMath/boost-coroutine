
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_SELF_H
#define BOOST_CORO_DETAIL_COROUTINE_SELF_H

#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/int.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/param_type.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename Result, int arity >
class coroutine_self;

template< typename Signature >
class coroutine_self< Signature, void, 0 >
{
private:
    typedef detail::coroutine_base<
        Signature, void, 0
    >                                           base_t;
    typedef base_t                         *    ptr_t;

    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    ptr_t                   impl_;
    context::fcontext_t **   callee_;

    coroutine_self( ptr_t impl, context::fcontext_t ** callee) BOOST_NOEXCEPT :
        impl_( impl), callee_( callee)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & yield()
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( ! impl_->is_complete() );

        context::fcontext_t caller;
        * callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            * callee_,
            ( intptr_t) & caller,
            fpu_preserved == impl_->preserve_fpu_);
        if ( impl_->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
        return * this;
    }
};

template< typename Signature >
class coroutine_self< Signature, void, 1 >
{
private:
    typedef typename arg< Signature >::type_t   arg_t;
    typedef detail::coroutine_base<
        Signature, void, 1
    >                                           base_t;
    typedef base_t                         *    ptr_t;

    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    ptr_t                   impl_;
    context::fcontext_t **   callee_;

    coroutine_self( ptr_t impl, context::fcontext_t ** callee) BOOST_NOEXCEPT :
        impl_( impl), callee_( callee)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & yield()
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( ! impl_->is_complete() );

        context::fcontext_t caller;
        * callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            * callee_,
            ( intptr_t) & caller,
            fpu_preserved == impl_->preserve_fpu_);
        if ( impl_->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
        return * this;
    }

    template< int N >
    arg_t get() const
    {
        BOOST_MPL_ASSERT((
            mpl::equal_to< mpl::int_< N >, mpl::int_< 0 > >
        ));
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( impl_->args_);

        return impl_->args_.get();
    }
};

template< typename Signature, int arity >
class coroutine_self< Signature, void, arity >
{
private:
    typedef typename arg< Signature >::type_t   arg_t;
    typedef detail::coroutine_base<
        Signature, void, arity
    >                                           base_t;
    typedef base_t                         *    ptr_t;

    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    ptr_t                   impl_;
    context::fcontext_t **   callee_;

    coroutine_self( ptr_t impl, context::fcontext_t ** callee) BOOST_NOEXCEPT :
        impl_( impl), callee_( callee)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & yield()
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( ! impl_->is_complete() );

        context::fcontext_t caller;
        * callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            * callee_,
            ( intptr_t) & caller,
            fpu_preserved == impl_->preserve_fpu_);
        if ( impl_->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
        return * this;
    }

    template< int N >
    typename tuples::element< N, arg_t >::type get() const
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( impl_->args_);

        return impl_->args_.get().get< N >();
    }
};

template< typename Signature, typename Result >
class coroutine_self< Signature, Result, 0 >
{
private:
    typedef detail::coroutine_base<
        Signature, Result, 0
    >                                           base_t;
    typedef base_t                         *    ptr_t;

    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    ptr_t                   impl_;
    context::fcontext_t **   callee_;

    coroutine_self( ptr_t impl, context::fcontext_t ** callee) BOOST_NOEXCEPT :
        impl_( impl), callee_( callee)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & yield( typename param_type< Result >::type param)
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( ! impl_->is_complete() );

        impl_->result_ = param;
        context::fcontext_t caller;
        * callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            * callee_,
            ( intptr_t) & caller,
            fpu_preserved == impl_->preserve_fpu_);
        if ( impl_->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
        return * this;
    }
};

template< typename Signature, typename Result >
class coroutine_self< Signature, Result, 1 >
{
private:
    typedef typename arg< Signature >::type_t   arg_t;
    typedef detail::coroutine_base<
        Signature, Result, 1
    >                                           base_t;
    typedef base_t                         *    ptr_t;

    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    ptr_t                   impl_;
    context::fcontext_t **   callee_;

    coroutine_self( ptr_t impl, context::fcontext_t ** callee) BOOST_NOEXCEPT :
        impl_( impl), callee_( callee)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & yield( typename param_type< Result >::type param)
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( ! impl_->is_complete() );

        impl_->result_ = param;
        context::fcontext_t caller;
        * callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            * callee_,
            ( intptr_t) & caller,
            fpu_preserved == impl_->preserve_fpu_);
        if ( impl_->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
        return * this;
    }

    template< int N >
    arg_t get() const
    {
        BOOST_MPL_ASSERT((
            mpl::equal_to< mpl::int_< N >, mpl::int_< 0 > >
        ));
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( impl_->args_);

        return impl_->args_.get();
    }
};

template< typename Signature, typename Result, int arity >
class coroutine_self
{
private:
    typedef typename arg< Signature >::type_t   arg_t;
    typedef detail::coroutine_base<
        Signature, Result, arity
    >                                           base_t;
    typedef base_t                         *    ptr_t;

    template< typename X, typename Y, typename R, int >
    friend class coroutine_exec;

    ptr_t                   impl_;
    context::fcontext_t **   callee_;

    coroutine_self( ptr_t impl, context::fcontext_t ** callee) BOOST_NOEXCEPT :
        impl_( impl), callee_( callee)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & yield( typename param_type< Result >::type param)
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( ! impl_->is_complete() );

        impl_->result_ = param;
        context::fcontext_t caller;
        * callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            * callee_,
            ( intptr_t) & caller,
            fpu_preserved == impl_->preserve_fpu_);
        if ( impl_->unwind_requested() )
            throw forced_unwind(); // BOOST_THROW_EXCEPTION?
        return * this;
    }

    template< int N >
    typename tuples::element< N, arg_t >::type get() const
    {
        BOOST_ASSERT( impl_);
        BOOST_ASSERT( impl_->args_);

        return impl_->args_.get().get< N >();
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_SELF_H
