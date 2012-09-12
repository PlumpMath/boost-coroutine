
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_GENERATOR_BASE_H
#define BOOST_CORO_DETAIL_GENERATOR_BASE_H

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
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/param_type.hpp>
#include <boost/coroutine/exceptions.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

struct forced_unwind_ {};

template< typename Context >
void trampoline_( intptr_t vp)
{
    BOOST_ASSERT( vp);

    Context * context( reinterpret_cast< Context * >( vp) );
    try
    { context->exec_(); }
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

template< typename Result >
class generator_base : private noncopyable
{
public:
    typedef intrusive_ptr< generator_base >   ptr_t;

private:
    template< typename T >
    friend void trampoline_( intptr_t);

    std::size_t             use_count_;
    ctx::fcontext_t         caller_;
    ctx::fcontext_t         callee_;
    int                     flags_;
    exception_ptr           except_;
    bool                    preserve_fpu_;

protected:
    template< typename StackAllocator >
    void deallocate_stack( StackAllocator & alloc) BOOST_NOEXCEPT
    {
        if ( ! is_complete()
                && ( is_started() || is_resumed() )
                && ( unwind_requested() ) )
            unwind_stack();
        alloc.deallocate( callee_.fc_stack.base, callee_.fc_stack.size);
    }

    virtual void exec_() = 0;
    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator >
    generator_base( attributes const& attr, StackAllocator const& alloc) :
        use_count_( 0),
        caller_(),
        callee_(),
        flags_( stack_unwind == attr.do_unwind ? flag_force_unwind : flag_dont_force_unwind),
        except_(),
        preserve_fpu_( attr.preserve_fpu)
    {
        callee_.fc_stack.base = alloc.allocate( attr.size);
        callee_.fc_stack.size = attr.size;
        ctx::make_fcontext( & callee_, trampoline_< generator_base>);
    }

    virtual ~generator_base()
    {}

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

    bool is_started() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_started); }

    bool is_resumed() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_resumed); }

    bool is_running() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_running); }

    void unwind_stack() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! is_complete() );
        BOOST_ASSERT( ! is_running() );

        flags_ |= flag_unwind_stack;
        ctx::jump_fcontext( & caller_, & callee_, 0, preserve_fpu_);
        flags_ &= ~flag_unwind_stack;
        BOOST_ASSERT( is_complete() );
    }

    void start( optional< Result > & result)
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

        if ( 0 != ret)
            result = * ( typename remove_reference< Result >::type *) ret;
        else
            result = none;
    }

    void resume( optional< Result > & result)
    {
        BOOST_ASSERT( is_started() );
        BOOST_ASSERT( ! is_complete() );
        BOOST_ASSERT( ! is_running() );

        flags_ |= flag_resumed;
        flags_ |= flag_running;
        intptr_t ret = ctx::jump_fcontext( & caller_, & callee_, 0, preserve_fpu_);
        flags_ &= ~flag_running;

        if ( 0 != ( flags_ & flag_has_exception) )
            rethrow_exception( except_);

        if ( 0 != ret)
            result = * ( typename remove_reference< Result >::type *) ret;
        else
            result = none;
    }

    void suspend( typename param_type< Result >::type param_)
    {
        BOOST_ASSERT( ! is_complete() );
        BOOST_ASSERT( is_running() );

        typename param_type< Result >::type param( param_);
        flags_ &= ~flag_running;
        ctx::jump_fcontext( & callee_, & caller_, ( intptr_t) & param, preserve_fpu_);
        if ( 0 != ( flags_ & flag_unwind_stack) )
            throw forced_unwind();
    }

    friend inline void intrusive_ptr_add_ref( generator_base * p) BOOST_NOEXCEPT
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( generator_base * p) BOOST_NOEXCEPT
    { if ( --p->use_count_ == 0) p->deallocate_object(); }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_GENERATOR_BASE_H
