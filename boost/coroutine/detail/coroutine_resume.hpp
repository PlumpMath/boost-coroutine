
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_RESUME_H
#define BOOST_CORO_DETAIL_COROUTINE_RESUME_H

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/context_base.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_resume;

template< typename Signature, typename D >
struct coroutine_resume< Signature, D, void, 0 >
{
    void operator()()
    {
		D * dp = static_cast< D * >( this);
        BOOST_ASSERT( dp->impl_);
		if ( ! dp->impl_->is_started() ) dp->impl_->start();
		else dp->impl_->resume();
    }
};

template< typename Signature, typename D, typename Result >
struct coroutine_resume< Signature, D, Result, 0 >
{
    typedef Result	result_t;

    result_t operator()()
    {
		D * dp = static_cast< D * >( this);
        BOOST_ASSERT( dp->impl_);
		if ( ! dp->impl_->is_started() ) return dp->impl_->start();
		else return dp->impl_->resume();
    }
};

#define BOOST_COROUTINE_RESUME_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_COROUTINE_RESUME_VAL(z,n,unused) BOOST_COROUTINE_RESUME_COMMA(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_RESUME_VALS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_RESUME_VAL,~)
#define BOOST_COROUTINE_RESUME_ARG_TYPE(n) \
	typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_COROUTINE_RESUME_ARG(z,n,unused) BOOST_COROUTINE_RESUME_COMMA(n) BOOST_COROUTINE_RESUME_ARG_TYPE(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_RESUME_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_RESUME_ARG,~)
#define BOOST_coroutine_resume(z,n,unused) \
template< typename Signature, typename D > \
struct coroutine_resume< Signature, D, void, n > \
{ \
    void operator()( BOOST_COROUTINE_RESUME_ARGS(n)) \
    { \
		D * dp = static_cast< D * >( this); \
        BOOST_ASSERT( dp->impl_); \
		if ( ! dp->impl_->is_started() ) \
			dp->impl_->start( BOOST_COROUTINE_RESUME_VALS(n)); \
		else \
			dp->impl_->resume( BOOST_COROUTINE_RESUME_VALS(n)); \
    } \
}; \
\
template< typename Signature, typename D, typename Result > \
struct coroutine_resume< Signature, D, Result, n > \
{ \
    typedef Result	result_t; \
\
    result_t operator()( BOOST_COROUTINE_RESUME_ARGS(n)) \
    { \
		D * dp = static_cast< D * >( this); \
        BOOST_ASSERT( dp->impl_); \
		if ( ! dp->impl_->is_started() ) \
			return dp->impl_->start( BOOST_COROUTINE_RESUME_VALS(n)); \
		else \
			return dp->impl_->resume( BOOST_COROUTINE_RESUME_VALS(n)); \
    } \
};
BOOST_PP_REPEAT_FROM_TO(1,11,BOOST_coroutine_resume,~)
#undef BOOST_COROUTINE_RESUME
#undef BOOST_COROUTINE_RESUME_ARGS
#undef BOOST_COROUTINE_RESUME_ARG
#undef BOOST_COROUTINE_RESUME_ARG_TYPE
#undef BOOST_COROUTINE_RESUME_VALS
#undef BOOST_COROUTINE_RESUME_VAL
#undef BOOST_COROUTINE_RESUME_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_RESUME_H
