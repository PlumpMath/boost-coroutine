
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_BASE_H
#define BOOST_CORO_DETAIL_COROUTINE_BASE_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/cstdint.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base_run.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Context >
struct holder
{
    Context             *   ctx;
    context::fcontext_t *   caller;

    holder( Context * ctx_, context::fcontext_t * caller_) :
        ctx( ctx_), caller( caller_)
    {}
};

template< typename Context >
void trampoline( intptr_t vp)
{
    BOOST_ASSERT( vp);

    holder< Context > * hldr( reinterpret_cast< holder< Context > * >( vp) );
    Context * ctx( hldr->ctx);
    context::fcontext_t * callee = ( context::fcontext_t *) context::jump_fcontext(
            ctx->callee_, hldr->caller, ( intptr_t) ctx->callee_, false);

    try
    { ctx->run_( & callee); }
    catch ( forced_unwind const&)
    {}
    catch (...)
    { ctx->except_ = current_exception(); }

    ctx->flags_ |= flag_complete;
    context::jump_fcontext(
        ctx->callee_, callee,
        ( intptr_t) ctx->callee_, fpu_preserved == ctx->preserve_fpu_);
}

template< typename Signature, typename Result, int arity >
class coroutine_base :
    private noncopyable,
    public coroutine_base_run<
        Signature, coroutine_base< Signature, Result, arity >, Result, arity
    >
{
public:
    typedef intrusive_ptr< coroutine_base >   ptr_t;

private:
    template< typename T >
    friend void trampoline( intptr_t);
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_base_run;
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_resume;
    template< typename X, typename Y, int >
    friend class coroutine_self;

    std::size_t             use_count_;
    std::size_t             size_;
    void                *   sp_;
    context::fcontext_t *   callee_;
    int                     flags_;
    exception_ptr           except_;
    bool                    preserve_fpu_;

protected:
    template< typename StackAllocator >
    void deallocate_stack( StackAllocator & alloc) BOOST_NOEXCEPT
    {
        if ( ! is_complete() && unwind_forced() ) unwind_stack();
        alloc.deallocate( sp_, size_);
    }

    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator >
    coroutine_base( attributes const& attr, StackAllocator const& alloc) :
        coroutine_base_run<
            Signature, coroutine_base< Signature, Result, arity >, Result, arity
        >(),
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        callee_(
            context::make_fcontext(
                sp_, size_, trampoline< coroutine_base>) ),
        flags_( stack_unwind == attr.do_unwind
            ? flag_force_unwind
            : flag_dont_force_unwind),
        except_(),
        preserve_fpu_( attr.preserve_fpu)
    {
        context::fcontext_t caller;
        holder< coroutine_base > hldr( this, & caller);
        callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller, callee_, ( intptr_t) & hldr, false);
        BOOST_ASSERT( callee_->fc_stack.size);
    }

    virtual ~coroutine_base()
    {}

    bool unwind_forced() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_unwind_stack); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

    void unwind_stack() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! is_complete() );

        flags_ |= flag_unwind_stack;
        context::fcontext_t caller;
        callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller, callee_,
            ( intptr_t) & caller, fpu_preserved == preserve_fpu_);
        flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( is_complete() );
    }

    friend inline void intrusive_ptr_add_ref( coroutine_base * p) BOOST_NOEXCEPT
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( coroutine_base * p) BOOST_NOEXCEPT
    { if ( --p->use_count_ == 0) p->deallocate_object(); }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_BASE_H
