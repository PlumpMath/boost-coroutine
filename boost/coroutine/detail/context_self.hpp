
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_CONTEXT_SELF_H
#define BOOST_CORO_DETAIL_CONTEXT_SELF_H

#include <boost/config.hpp>
#include <boost/throw_exception.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/context_base.hpp>
#include <boost/coroutine/detail/param_type.hpp>
#include <boost/coroutine/exceptions.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename Allocator, typename Result, int arity >
class context_self;

template< typename Signature, typename Allocator >
class context_self< Signature, Allocator, void, 0 >
{
private:
    typedef detail::context_base<
        Signature, Allocator, void, 0
    >                                       	base_t;
	typedef base_t                         *	ptr_t; 

	template< typename X, typename Y, typename Z, int >
    friend class event_object;
	template< typename X, typename Y, typename Z, typename R, int >
	friend class context_exec;
    
	ptr_t  impl_;

    context_self( ptr_t impl) :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    void yield()
    {
        BOOST_ASSERT( impl_);
        impl_->suspend();
    }

    void yield_break()
    {
        BOOST_ASSERT( impl_);
        BOOST_THROW_EXCEPTION( coroutine_terminated() );
    }
};

template< typename Signature, typename Allocator, int arity >
class context_self< Signature, Allocator, void, arity >
{
private:
	typedef typename arg< Signature >::type_t	arg_t;
    typedef detail::context_base<
        Signature, Allocator, void, arity
    >                                       	base_t;
	typedef base_t                         *	ptr_t; 

	template< typename X, typename Y, typename Z, int >
    friend class event_object;
	template< typename X, typename Y, typename Z, typename R, int >
	friend class context_exec;
    
	ptr_t  impl_;

    context_self( ptr_t impl) :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    arg_t yield()
    {
        BOOST_ASSERT( impl_);
        return impl_->suspend();
    }

    void yield_break()
    {
        BOOST_ASSERT( impl_);
        BOOST_THROW_EXCEPTION( coroutine_terminated() );
    }
};

template< typename Signature, typename Allocator, typename Result >
class context_self< Signature, Allocator, Result, 0 >
{
private:
	typedef Result								result_t;
    typedef detail::context_base<
        Signature, Allocator, Result, 0
    >                                       	base_t;
	typedef base_t                         *	ptr_t; 

	template< typename X, typename Y, typename Z, int >
    friend class event_object;
	template< typename X, typename Y, typename Z, typename R, int >
	friend class context_exec;
    
	ptr_t  impl_;

    context_self( ptr_t impl) :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    void yield( typename param_type< result_t >::type param)
    {
        BOOST_ASSERT( impl_);
        impl_->suspend( param);
    }

    void yield_break()
    {
        BOOST_ASSERT( impl_);
        BOOST_THROW_EXCEPTION( coroutine_terminated() );
    }
};

template< typename Signature, typename Allocator, typename Result, int arity >
class context_self
{
private:
	typedef Result								result_t;
	typedef typename arg< Signature >::type_t	arg_t;
    typedef detail::context_base<
        Signature, Allocator, Result, arity
    >                                       	base_t;
	typedef base_t                         *	ptr_t; 

	template< typename X, typename Y, typename Z, int >
    friend class event_object;
	template< typename X, typename Y, typename Z, typename R, int >
	friend class context_exec;
    
	ptr_t  impl_;

    context_self( ptr_t impl) :
        impl_( impl)
    { BOOST_ASSERT( impl_); }

public:
    arg_t yield( typename param_type< result_t >::type param)
    {
        BOOST_ASSERT( impl_);
        return impl_->suspend( param);
    }

    void yield_break()
    {
        BOOST_ASSERT( impl_);
        BOOST_THROW_EXCEPTION( coroutine_terminated() );
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_CONTEXT_SELF_H
