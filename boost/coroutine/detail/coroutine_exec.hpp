
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_EXEC_H
#define BOOST_CORO_DETAIL_COROUTINE_EXEC_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/holder.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity, typename Caller >
class coroutine_exec;

template< typename Signature, typename D, typename Caller >
struct coroutine_exec< Signature, D, void, 0, Caller > :
    public coroutine_base< Signature, void, 0 >
{
    template< typename StackAllocator >
    coroutine_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT :
        coroutine_base< Signature, void, 0 >( attr, alloc, this)
    {}

    void run( context::fcontext_t * callee)
    {
        callee = ( context::fcontext_t *) context::jump_fcontext(
                this->callee_, callee, ( intptr_t) this->callee_, this->preserve_fpu_);

        Caller c( callee, this->preserve_fpu_, static_cast< D const* >( this)->alloc_);
        try
        {
            context::fcontext_t caller;
            static_cast< D const* >( this)->fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            context::jump_fcontext(
                    & caller, callee,
                    ( intptr_t) & caller, fpu_preserved == this->preserve_fpu_);
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        context::fcontext_t caller;
        context::jump_fcontext(
                & caller, callee,
                ( intptr_t) & caller, fpu_preserved == this->preserve_fpu_);
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }
};

template< typename Signature, typename D, typename Result, typename Caller >
struct coroutine_exec< Signature, D, Result, 0, Caller > :
    public coroutine_base< Signature, Result, 0 >
{
    template< typename StackAllocator >
    coroutine_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT :
        coroutine_base< Signature, Result, 0 >( attr, alloc, this)
    {}

    void run( context::fcontext_t * callee)
    {
        callee = ( context::fcontext_t *) context::jump_fcontext(
                this->callee_, callee, ( intptr_t) this->callee_, this->preserve_fpu_);

        Caller c( callee, this->preserve_fpu_, static_cast< D const* >( this)->alloc_);
        try
        {
            context::fcontext_t caller;
            holder< Result > hldr( & caller,
                                   static_cast< D * >( this)->fn_( c) );
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            context::jump_fcontext(
                    hldr.ctx, callee,
                    ( intptr_t) & hldr, fpu_preserved == this->preserve_fpu_);
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        context::fcontext_t caller;
        context::jump_fcontext(
                & caller, callee,
                ( intptr_t) & caller, fpu_preserved == this->preserve_fpu_);
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }
};

#define BOOST_CONTEXT_EXEC_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_CONTEXT_EXEC_VAL(z,n,unused) BOOST_CONTEXT_EXEC_COMMA(n) BOOST_PP_CAT(a,n)
#define BOOST_CONTEXT_EXEC_VALS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_CONTEXT_EXEC_VAL,~)
#define BOOST_CONTEXT_EXEC_ARG_TYPE(n) \
    typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_CONTEXT_EXEC_ARG(z,n,unused) BOOST_CONTEXT_EXEC_COMMA(n) BOOST_CONTEXT_EXEC_ARG_TYPE(n) BOOST_PP_CAT(a,n)
#define BOOST_CONTEXT_EXEC_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_CONTEXT_EXEC_ARG,~)
#define BOOST_CONTEXT_EXEC(z,n,unused) \
template< typename Signature, typename D, typename Caller > \
struct coroutine_exec< Signature, D, void, n, Caller > : \
    public coroutine_base< Signature, void, n > \
{ \
    typedef typename arg< Signature >::type_t   arg_t; \
\
    void run( context::fcontext_t * callee) \
    { \
        holder< arg_t > * hldr = ( holder< arg_t > *) context::jump_fcontext( \
                this->callee_, callee, ( intptr_t) this->callee_, this->preserve_fpu_); \
        callee = hldr->ctx; \
\
        Caller c( callee, this->preserve_fpu_, static_cast< D const* >( this)->alloc_); \
        c.impl_->result_ = hldr->data; \
        try \
        { \
            context::fcontext_t caller; \
            static_cast< D const* >( this)->fn_( c); \
            this->flags_ |= flag_complete; \
            callee = c.impl_->callee_; \
            context::jump_fcontext( \
                    & caller, callee, \
                    ( intptr_t) & caller, fpu_preserved == this->preserve_fpu_); \
            BOOST_ASSERT_MSG( false, "coroutine is complete"); \
        } \
        catch ( forced_unwind const&) \
        {} \
        catch (...) \
        { this->except_ = current_exception(); } \
\
        this->flags_ |= flag_complete; \
        callee = c.impl_->callee_; \
        context::fcontext_t caller; \
        context::jump_fcontext( \
                & caller, callee, \
                ( intptr_t) & caller, fpu_preserved == this->preserve_fpu_); \
        BOOST_ASSERT_MSG( false, "coroutine is complete"); \
    } \
\
    template< typename StackAllocator > \
    coroutine_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT : \
        coroutine_base< Signature, void, n >( attr, alloc, this) \
    {} \
}; \
\
template< typename Signature, typename D, typename Result, typename Caller > \
struct coroutine_exec< Signature, D, Result, n, Caller > : \
    public coroutine_base< Signature, Result, n > \
{ \
    typedef typename arg< Signature >::type_t   arg_t; \
\
    template< typename StackAllocator > \
    coroutine_exec( attributes const& attr, StackAllocator const& alloc) BOOST_NOEXCEPT : \
        coroutine_base< Signature, Result, n >( attr, alloc, this) \
    {} \
\
    void run( context::fcontext_t * callee) \
    { \
        holder< arg_t > * hldr = ( holder< arg_t > *) context::jump_fcontext( \
                this->callee_, callee, ( intptr_t) this->callee_, this->preserve_fpu_); \
        callee = hldr->ctx; \
\
        Caller c( callee, this->preserve_fpu_, static_cast< D const* >( this)->alloc_); \
        c.impl_->result_ = hldr->data; \
        try \
        { \
            context::fcontext_t caller; \
            holder< Result > hldr( & caller, \
                                   static_cast< D * >( this)->fn_( c) ); \
            this->flags_ |= flag_complete; \
            callee = c.impl_->callee_; \
            context::jump_fcontext( \
                    hldr.ctx, callee, \
                    ( intptr_t) & hldr, fpu_preserved == this->preserve_fpu_); \
            BOOST_ASSERT_MSG( false, "coroutine is complete"); \
        } \
        catch ( forced_unwind const&) \
        {} \
        catch (...) \
        { this->except_ = current_exception(); } \
\
        this->flags_ |= flag_complete; \
        callee = c.impl_->callee_; \
        context::fcontext_t caller; \
        context::jump_fcontext( \
                & caller, callee, \
                ( intptr_t) & caller, fpu_preserved == this->preserve_fpu_); \
        BOOST_ASSERT_MSG( false, "coroutine is complete"); \
    } \
};
BOOST_PP_REPEAT_FROM_TO(1,11,BOOST_CONTEXT_EXEC,~)
#undef BOOST_CONTEXT_EXEC
#undef BOOST_CONTEXT_EXEC_ARGS
#undef BOOST_CONTEXT_EXEC_ARG
#undef BOOST_CONTEXT_EXEC_ARG_TYPE
#undef BOOST_CONTEXT_EXEC_VALS
#undef BOOST_CONTEXT_EXEC_VAL
#undef BOOST_CONTEXT_EXEC_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_EXEC_H
