
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_BASE_START_H
#define BOOST_CORO_DETAIL_COROUTINE_BASE_START_H

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/throw_exception.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/exceptions.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_base_start;

template< typename Signature, typename D >
struct coroutine_base_start< Signature, D, void, 0 >
{
    void start()
    {
        D * dp = static_cast< D * >( this);

        dp->native_start();
    }
};

template< typename Signature, typename D, typename Result >
struct coroutine_base_start< Signature, D, Result, 0 >
{
    typedef Result  result_t;

    result_t start()
    {
        D * dp = static_cast< D * >( this);

        intptr_t ret = dp->native_start();
        if ( dp->is_complete() ) return * dp->result_;
        else return * ( typename remove_reference< result_t >::type *) ret;
    }
};

#define BOOST_CONTEXT_BASE_START_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_CONTEXT_BASE_START_VAL(z,n,unused) BOOST_CONTEXT_BASE_START_COMMA(n) BOOST_PP_CAT(a,n)
#define BOOST_CONTEXT_BASE_START_VALS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_CONTEXT_BASE_START_VAL,~)
#define BOOST_CONTEXT_BASE_START_ARG_TYPE(n) \
    typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_CONTEXT_BASE_START_ARG(z,n,unused) BOOST_CONTEXT_BASE_START_COMMA(n) BOOST_CONTEXT_BASE_START_ARG_TYPE(n) BOOST_PP_CAT(a,n)
#define BOOST_CONTEXT_BASE_START_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_CONTEXT_BASE_START_ARG,~)
#define BOOST_CONTEXT_BASE_START(z,n,unused) \
template< typename Signature, typename D > \
struct coroutine_base_start< Signature, D, void, n > \
{ \
    void start( BOOST_CONTEXT_BASE_START_ARGS(n)) \
    { \
        D * dp = static_cast< D * >( this); \
\
        dp->args_ = typename arg< Signature >::type_t(BOOST_CONTEXT_BASE_START_VALS(n)); \
        dp->native_start(); \
    } \
}; \
\
template< typename Signature, typename D, typename Result > \
struct coroutine_base_start< Signature, D, Result, n > \
{ \
    typedef Result  result_t; \
\
    result_t start( BOOST_CONTEXT_BASE_START_ARGS(n)) \
    { \
        D * dp = static_cast< D * >( this); \
\
        dp->args_ = typename arg< Signature >::type_t(BOOST_CONTEXT_BASE_START_VALS(n)); \
        intptr_t ret = dp->native_start(); \
        if ( dp->is_complete() ) return * dp->result_; \
        else return * ( typename remove_reference< result_t >::type *) ret; \
    } \
};
BOOST_PP_REPEAT_FROM_TO(1,11,BOOST_CONTEXT_BASE_START,~)
#undef BOOST_CONTEXT_BASE_START
#undef BOOST_CONTEXT_BASE_START_ARGS
#undef BOOST_CONTEXT_BASE_START_ARG
#undef BOOST_CONTEXT_BASE_START_ARG_TYPE
#undef BOOST_CONTEXT_BASE_START_VALS
#undef BOOST_CONTEXT_BASE_START_VAL
#undef BOOST_CONTEXT_BASE_START_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_BASE_START_H
