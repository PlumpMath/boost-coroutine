
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_CONTEXT_BASE_H
#define BOOST_CORO_DETAIL_CONTEXT_BASE_H

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

#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/context_base_resume.hpp>
#include <boost/coroutine/detail/context_base_run.hpp>
#include <boost/coroutine/detail/context_base_start.hpp>
#include <boost/coroutine/detail/context_base_suspend.hpp>
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

    Context * context( reinterpret_cast< Context * >( vp) );
    try
    { context->run_(); }
    catch ( forced_unwind const&)
    {}
    catch (...)
    {
        context->flags_ |= flag_has_exception;
        context->except_ = current_exception();
    }
    context->flags_ |= flag_complete;
    ctx::jump_fcontext( & context->callee_, & context->caller_, 0, context->preserve_fpu_);
}

template< typename Signature, typename Allocator, typename Result, int arity >
class context_base :
	private noncopyable,
	public context_base_start<
        Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
    >,
	public context_base_resume<
        Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
    >,
	public context_base_suspend<
        Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
    >,
	public context_base_run<
        Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
    >
{
public:
    typedef intrusive_ptr< context_base >	ptr_t;

private:
    template< typename T >
    friend void trampoline( intptr_t);
    template< typename X, typename Y, typename Z, int >
    friend struct context_base_resume;
    template< typename X, typename Y, typename Z, int >
    friend struct context_base_run;
    template< typename X, typename Y, typename Z, int >
    friend struct context_base_start;
    template< typename X, typename Y, typename Z, int >
    friend struct context_base_suspend;

    std::size_t		    use_count_;
	Allocator		    alloc_;
    ctx::fcontext_t     caller_;
    ctx::fcontext_t     callee_;
    int                 flags_;
    exception_ptr       except_;
    bool                preserve_fpu_;

public:
    context_base( Allocator const& alloc, std::size_t size,
                  flag_unwind_t do_unwind, bool preserve_fpu) :
		context_base_start<
            Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
        >(),
		context_base_resume<
            Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
        >(),
		context_base_suspend<
            Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
        >(),
		context_base_run<
            Signature, context_base< Signature, Allocator, Result, arity >, Result, arity
        >(),
        use_count_( 0), alloc_( alloc), 
		caller_(),
        callee_(),
        flags_( stack_unwind == do_unwind ? flag_force_unwind : flag_dont_force_unwind),
        except_(),
        preserve_fpu_( preserve_fpu)
    {
        callee_.fc_stack.base = alloc_.allocate( size);
        callee_.fc_stack.size = size;
        ctx::make_fcontext( & callee_, trampoline< context_base>);
    }

	virtual ~context_base()
    {
        if ( ! is_complete()
                && ( is_started() || is_resumed() )
                && ( unwind_requested() ) )
            unwind_stack();
		alloc_.deallocate( callee_.fc_stack.base, callee_.fc_stack.size);
    }

    bool unwind_requested() const
    { return 0 != ( flags_ & flag_force_unwind); }

    bool is_complete() const
    { return 0 != ( flags_ & flag_complete); }

    bool is_started() const
    { return 0 != ( flags_ & flag_started); }

    bool is_resumed() const
    { return 0 != ( flags_ & flag_resumed); }

    bool is_running() const
    { return 0 != ( flags_ & flag_running); }

    void unwind_stack()
    {
        BOOST_ASSERT( ! is_complete() );
        BOOST_ASSERT( ! is_running() );

        flags_ |= flag_unwind_stack;
        ctx::jump_fcontext( & caller_, & callee_, 0, preserve_fpu_);
        flags_ &= ~flag_unwind_stack;
        BOOST_ASSERT( is_complete() );
    }

    intptr_t native_start()
    {
        BOOST_ASSERT( ! is_complete() );
        BOOST_ASSERT( ! is_started() );
        BOOST_ASSERT( ! is_running() );

        flags_ |= flag_started;
        flags_ |= flag_running;
        intptr_t ret = ctx::jump_fcontext( & caller_, & callee_, ( intptr_t) this, preserve_fpu_);
        flags_ &= ~flag_running;
        if ( 0 != ( flags_ & flag_has_exception) )
            rethrow_exception( except_);
        return ret; 
    }

    intptr_t native_resume( intptr_t param)
    {
        BOOST_ASSERT( is_started() );
        BOOST_ASSERT( ! is_complete() );
        BOOST_ASSERT( ! is_running() );

        flags_ |= flag_resumed;
        flags_ |= flag_running;
        intptr_t ret = ctx::jump_fcontext( & caller_, & callee_, param, preserve_fpu_);
        flags_ &= ~flag_running;
        if ( 0 != ( flags_ & flag_has_exception) )
            rethrow_exception( except_);
        return ret; 
    }

    intptr_t native_suspend( intptr_t param)
    {
        BOOST_ASSERT( ! is_complete() );
        BOOST_ASSERT( is_running() );

        flags_ &= ~flag_running;
        intptr_t ret = ctx::jump_fcontext( & callee_, & caller_, param, preserve_fpu_);
        if ( 0 != ( flags_ & flag_unwind_stack) )
            throw forced_unwind();
		return ret;
    }

    friend inline void intrusive_ptr_add_ref( context_base * p)
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( context_base * p)
    { if ( --p->use_count_ == 0) delete p; }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_CONTEXT_BASE_H
