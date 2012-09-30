
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_SELF_H
#define BOOST_CORO_DETAIL_COROUTINE_SELF_H

#include <boost/config.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/int.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/param_type.hpp>
#include <boost/coroutine/exceptions.hpp>

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

    ptr_t  impl_;

    coroutine_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & operator()()
    {
        BOOST_ASSERT( impl_);
        impl_->suspend();
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

    ptr_t  impl_;

    coroutine_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & operator()()
    {
        BOOST_ASSERT( impl_);
        impl_->suspend();
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

    ptr_t  impl_;

    coroutine_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & operator()()
    {
        BOOST_ASSERT( impl_);
        impl_->suspend();
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

    ptr_t  impl_;

    coroutine_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & operator()( typename param_type< Result >::type param)
    {
        BOOST_ASSERT( impl_);
        impl_->suspend( param);
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

    ptr_t  impl_;

    coroutine_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & operator()( typename param_type< Result >::type param)
    {
        BOOST_ASSERT( impl_);
        impl_->suspend( param);
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

    ptr_t  impl_;

    coroutine_self( ptr_t impl) BOOST_NOEXCEPT :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    coroutine_self & operator()( typename param_type< Result >::type param)
    {
        BOOST_ASSERT( impl_);
        impl_->suspend( param);
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
