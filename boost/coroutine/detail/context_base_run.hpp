
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_CONTEXT_BASE_RUN_H
#define BOOST_CORO_DETAIL_CONTEXT_BASE_RUN_H

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/optional.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
struct context_base_run;

template< typename Signature, typename D >
struct context_base_run< Signature, D, void, 0 >
{
    void run_()
    {
		D * dp = static_cast< D * >( this);

		dp->exec_();	
    }

	virtual void exec_() = 0;
};

template< typename Signature, typename D, typename Result >
struct context_base_run< Signature, D, Result, 0 >
{
	typedef Result			result_t;

	optional< result_t >	result_;

	context_base_run() :
		result_()
	{}

    void run_()
    {
		D * dp = static_cast< D * >( this);

		result_ = dp->exec_();	
    }

	virtual result_t exec_() = 0;
};

template< typename Signature, typename D >
struct context_base_run< Signature, D, void, 1 >
{
	typedef typename arg< Signature >::type_t	arg_t; \

	optional< arg_t > 	args_;

	context_base_run() :
		args_()
	{}

    void run_()
    {
		D * dp = static_cast< D * >( this);

		dp->exec_( * args_);
    }

	virtual void exec_( arg_t) = 0;
};

template< typename Signature, typename D, typename Result >
struct context_base_run< Signature, D, Result, 1 >
{
	typedef typename arg< Signature >::type_t	arg_t;
	typedef Result								result_t;

	optional< arg_t >		args_;
	optional< result_t >	result_;

	context_base_run() :
		args_(), result_()
	{}

    void run_()
    {
		D * dp = static_cast< D * >( this);

		result_ = dp->exec_( * args_);	
    }

	virtual result_t exec_( arg_t) = 0;
};

#define BOOST_CONTEXT_BASE_RUN_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_CONTEXT_BASE_RUN_VAL(z,n,unused) args_->get< n >()
#define BOOST_CONTEXT_BASE_RUN_VALS(n) BOOST_PP_ENUM(n,BOOST_CONTEXT_BASE_RUN_VAL,~)
#define BOOST_CONTEXT_BASE_RUN_ARG_TYPE(n) \
	typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_CONTEXT_BASE_RUN_ARG(z,n,unused) BOOST_CONTEXT_BASE_RUN_COMMA(n) BOOST_CONTEXT_BASE_RUN_ARG_TYPE(n)
#define BOOST_CONTEXT_BASE_RUN_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_CONTEXT_BASE_RUN_ARG,~)
#define BOOST_CONTEXT_BASE_RUN(z,n,unused) \
template< typename Signature, typename D > \
struct context_base_run< Signature, D, void, n > \
{ \
	typedef typename arg< Signature >::type_t	arg_t; \
\
	optional< arg_t >		args_; \
\
	context_base_run() : \
		args_() \
	{} \
\
    void run_() \
    { \
		D * dp = static_cast< D * >( this); \
\
		dp->exec_(BOOST_CONTEXT_BASE_RUN_VALS(n)); \
    } \
\
	virtual void exec_(BOOST_CONTEXT_BASE_RUN_ARGS(n)) = 0; \
}; \
\
template< typename Signature, typename D, typename Result > \
struct context_base_run< Signature, D, Result, n > \
{ \
    typedef Result								result_t; \
	typedef typename arg< Signature >::type_t	arg_t; \
\
	optional< arg_t >		args_; \
	optional< result_t >	result_; \
\
	context_base_run() : \
		args_(), result_() \
	{} \
\
    void run_() \
    { \
		D * dp = static_cast< D * >( this); \
\
		result_ = dp->exec_(BOOST_CONTEXT_BASE_RUN_VALS(n)); \
    } \
\
	virtual result_t exec_(BOOST_CONTEXT_BASE_RUN_ARGS(n)) = 0; \
};
BOOST_PP_REPEAT_FROM_TO(2,11,BOOST_CONTEXT_BASE_RUN,~)
#undef BOOST_CONTEXT_BASE_RUN
#undef BOOST_CONTEXT_BASE_RUN_ARGS
#undef BOOST_CONTEXT_BASE_RUN_ARG
#undef BOOST_CONTEXT_BASE_RUN_ARG_TYPE
#undef BOOST_CONTEXT_BASE_RUN_VALS
#undef BOOST_CONTEXT_BASE_RUN_VAL
#undef BOOST_CONTEXT_BASE_RUN_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_CONTEXT_BASE_RUN_H
