
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_BASE_RESUME_H
#define BOOST_CORO_DETAIL_COROUTINE_BASE_RESUME_H

#include <iterator>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/throw_exception.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/optional.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/range.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/holder.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_base_resume;

template< typename Signature, typename D >
struct coroutine_base_resume< Signature, D, void, 0 >
{
    void resume()
    {
        context::fcontext_t caller;
        static_cast< D * >( this)->callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            static_cast< D * >( this)->callee_,
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->preserve_fpu_);
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }
};

template< typename Signature, typename D, typename Result >
class coroutine_base_resume< Signature, D, Result, 0 >
{
public:
    void resume()
    {
        BOOST_ASSERT( static_cast< D * >( this));
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() );

        context::fcontext_t caller;
        holder< Result > * hldr = ( holder< Result > *) context::jump_fcontext(
            & caller,
            static_cast< D * >( this)->callee_,
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->preserve_fpu_);
        static_cast< D * >( this)->callee_ = hldr->ctx;
        static_cast< D * >( this)->result_ = hldr->data;
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_get;
    template< typename X, typename Y, typename Z, int, typename C >
    friend struct coroutine_exec;

    optional< Result >  result_;
};

template< typename Signature, typename D >
struct coroutine_base_resume< Signature, D, void, 1 >
{
    typedef typename arg< Signature >::type_t   arg_t;

    void resume( arg_t a1)
    {
        BOOST_ASSERT( static_cast< D * >( this));
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() );

        context::fcontext_t caller;
        holder< arg_t > hldr_to( & caller, a1);
        static_cast< D * >( this)->callee_ = ( context::fcontext_t *) context::jump_fcontext(
            hldr_to.ctx,
            static_cast< D * >( this)->callee_,
            ( intptr_t) & hldr_to, fpu_preserved == static_cast< D * >( this)->preserve_fpu_);
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }
};

template< typename Signature, typename D, typename Result >
class coroutine_base_resume< Signature, D, Result, 1 >
{
public:
    typedef typename arg< Signature >::type_t   arg_t;

    void resume( arg_t a1)
    {
        BOOST_ASSERT( static_cast< D * >( this));
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() );

        context::fcontext_t caller;
        holder< arg_t > hldr_to( & caller, a1); 
        holder< Result > * hldr_from = ( holder< Result > *) context::jump_fcontext(
            hldr_to.ctx,
            static_cast< D * >( this)->callee_,
            ( intptr_t) & hldr_to, fpu_preserved == static_cast< D * >( this)->preserve_fpu_);
        static_cast< D * >( this)->callee_ = hldr_from->ctx;
        static_cast< D * >( this)->result_ = hldr_from->data;
        if ( static_cast< D * >( this)->except_)
            rethrow_exception( static_cast< D * >( this)->except_);
    }

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_get;
    template< typename X, typename Y, typename Z, int, typename C >
    friend struct coroutine_exec;

    optional< Result >  result_;
};

#define BOOST_COROUTINE_BASE_RESUME_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_COROUTINE_BASE_RESUME_VAL(z,n,unused) BOOST_COROUTINE_BASE_RESUME_COMMA(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_BASE_RESUME_VALS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_BASE_RESUME_VAL,~)
#define BOOST_COROUTINE_BASE_RESUME_ARG_TYPE(n) \
    typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_COROUTINE_BASE_RESUME_ARG(z,n,unused) BOOST_COROUTINE_BASE_RESUME_COMMA(n) BOOST_COROUTINE_BASE_RESUME_ARG_TYPE(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_BASE_RESUME_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_BASE_RESUME_ARG,~)
#define BOOST_COROUTINE_BASE_RESUME(z,n,unused) \
template< typename Signature, typename D > \
struct coroutine_base_resume< Signature, D, void, n > \
{ \
    typedef typename arg< Signature >::type_t   arg_t; \
\
    void resume( BOOST_COROUTINE_BASE_RESUME_ARGS(n)) \
    { \
        BOOST_ASSERT( static_cast< D * >( this)); \
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() ); \
\
        context::fcontext_t caller; \
        holder< arg_t > hldr_to( & caller, arg_t(BOOST_COROUTINE_BASE_RESUME_VALS(n) ) ); \
        static_cast< D * >( this)->callee_ = ( context::fcontext_t *) context::jump_fcontext( \
            hldr_to.ctx, \
            static_cast< D * >( this)->callee_, \
            ( intptr_t) & hldr_to, fpu_preserved == static_cast< D * >( this)->preserve_fpu_); \
        if ( static_cast< D * >( this)->except_) \
            rethrow_exception( static_cast< D * >( this)->except_); \
    } \
}; \
\
template< typename Signature, typename D, typename Result > \
class coroutine_base_resume< Signature, D, Result, n > \
{ \
public: \
    typedef typename arg< Signature >::type_t   arg_t; \
\
    void resume( BOOST_COROUTINE_BASE_RESUME_ARGS(n)) \
    { \
        BOOST_ASSERT( static_cast< D * >( this)); \
        BOOST_ASSERT( ! static_cast< D * >( this)->is_complete() ); \
\
        context::fcontext_t caller; \
        holder< arg_t > hldr_to( & caller, arg_t(BOOST_COROUTINE_BASE_RESUME_VALS(n) ) ); \
        holder< Result > * hldr_from = ( holder< Result > *) context::jump_fcontext( \
            hldr_to.ctx, \
            static_cast< D * >( this)->callee_, \
            ( intptr_t) & hldr_to, \
            fpu_preserved == static_cast< D * >( this)->preserve_fpu_); \
        static_cast< D * >( this)->callee_ = hldr_from->ctx; \
        static_cast< D * >( this)->result_ = hldr_from->data; \
        if ( static_cast< D * >( this)->except_) \
            rethrow_exception( static_cast< D * >( this)->except_); \
    } \
\
private: \
    template< typename X, typename Y, typename Z, int > \
    friend struct coroutine_get; \
    template< typename X, typename Y, typename Z, int, typename C > \
    friend struct coroutine_exec; \
\
    optional< Result >  result_; \
};
BOOST_PP_REPEAT_FROM_TO(2,11,BOOST_COROUTINE_BASE_RESUME,~)
#undef BOOST_COROUTINE_BASE_RESUME
#undef BOOST_COROUTINE_BASE_RESUME_ARGS
#undef BOOST_COROUTINE_BASE_RESUME_ARG
#undef BOOST_COROUTINE_BASE_RESUME_ARG_TYPE
#undef BOOST_COROUTINE_BASE_RESUME_VALS
#undef BOOST_COROUTINE_BASE_RESUME_VAL
#undef BOOST_COROUTINE_BASE_RESUME_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_coroutine_base_resume_H
