
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_BASE_H
#define BOOST_CORO_DETAIL_COROUTINE_BASE_H

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/cstdint.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base_run.hpp>
#include <boost/coroutine/detail/coroutine_base_suspend.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/exceptions.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

struct forced_unwind {};

template< typename Context >
void trampoline( intptr_t vp)
{
    BOOST_ASSERT( vp);

    Context * ctx( reinterpret_cast< Context * >( vp) );
    context::jump_fcontext( ctx->callee_, & ctx->caller_, 0, false);

    try
    { ctx->run_(); }
    catch ( forced_unwind const&)
    {}
    catch (...)
    { ctx->except_ = current_exception(); }
    ctx->flags_ |= flag_complete;
    context::jump_fcontext( ctx->callee_, & ctx->caller_, 0, fpu_preserved == ctx->preserve_fpu_);
}

template< typename Signature, typename Result, int arity >
class coroutine_base :
    private noncopyable,
    public coroutine_base_suspend<
        Signature, coroutine_base< Signature, Result, arity >, Result, arity
    >,
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
    friend struct coroutine_base_suspend;

    std::size_t         use_count_;
    context::fcontext_t     caller_;
    context::fcontext_t *   callee_;
    int                 flags_;
    exception_ptr       except_;
    bool                preserve_fpu_;

protected:
    template< typename StackAllocator >
    void deallocate_stack( StackAllocator & alloc) BOOST_NOEXCEPT
    {
        if ( ! is_complete()
                && ( unwind_requested() ) )
            unwind_stack();
        alloc.deallocate( callee_->fc_stack.sp, callee_->fc_stack.size);
    }

    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator >
    coroutine_base( attributes const& attr, StackAllocator const& alloc) :
        coroutine_base_suspend<
            Signature, coroutine_base< Signature, Result, arity >, Result, arity
        >(),
        coroutine_base_run<
            Signature, coroutine_base< Signature, Result, arity >, Result, arity
        >(),
        use_count_( 0),
        caller_(),
        callee_(
            context::make_fcontext(
                alloc.allocate( attr.size), attr.size, trampoline< coroutine_base>) ),
        flags_( stack_unwind == attr.do_unwind ? flag_force_unwind : flag_dont_force_unwind),
        except_(),
        preserve_fpu_( attr.preserve_fpu)
    { context::jump_fcontext( & caller_, callee_, ( intptr_t) this, false); }

    virtual ~coroutine_base()
    {}

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

    void unwind_stack() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! is_complete() );

        flags_ |= flag_unwind_stack;
        context::jump_fcontext( & caller_, callee_, 0, fpu_preserved == preserve_fpu_);
        flags_ &= ~flag_unwind_stack;
        BOOST_ASSERT( is_complete() );
    }

    intptr_t native_resume( intptr_t param)
    {
        BOOST_ASSERT( ! is_complete() );

        intptr_t ret = context::jump_fcontext( & caller_, callee_, param, fpu_preserved == preserve_fpu_);
        if ( except_)
            rethrow_exception( except_);
        return ret; 
    }

    intptr_t native_suspend( intptr_t param)
    {
        BOOST_ASSERT( ! is_complete() );

        intptr_t ret = context::jump_fcontext( callee_, & caller_, param, fpu_preserved == preserve_fpu_);
        if ( 0 != ( flags_ & flag_unwind_stack) )
            throw forced_unwind();
        return ret;
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
