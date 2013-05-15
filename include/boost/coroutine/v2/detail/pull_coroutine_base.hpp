
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_V2_DETAIL_PULL_COROUTINE_BASE_H
#define BOOST_COROUTINES_V2_DETAIL_PULL_COROUTINE_BASE_H

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_context.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/param.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {

struct stack_context;

namespace detail {

template< typename R >
class pull_coroutine_base : private noncopyable,
{
public:
    typedef intrusive_ptr< pull_coroutine_base >     ptr_t;

private:
    unsigned int        use_count_;
    int                 flags_;
    exception_ptr       except_;

protected:
    coroutine_context   caller_;
    coroutine_context   callee_;
    optional< R >       result_;

    virtual void deallocate_object() = 0;

public:
    pull_coroutine_base( coroutine_context::ctx_fn fn,
                         stack_context * stack_ctx,
                         bool unwind, bool preserve_fpu) :
        use_count_( 0),
        flags_( 0),
        except_(),
        caller_(),
        callee_( fn, stack_ctx),
        result_()
    {
        if ( unwind) flags_ |= flag_force_unwind;
        if ( preserve_fpu) flags_ |= flag_preserve_fpu;
    }

    pull_coroutine_base( coroutine_context const& callee,
                         bool unwind, bool preserve_fpu) :
        use_count_( 0),
        flags_( 0),
        except_(),
        caller_(),
        callee_( callee),
        result_()
    {
        if ( unwind) flags_ |= flag_force_unwind;
        if ( preserve_fpu) flags_ |= flag_preserve_fpu;
    }

    virtual ~pull_coroutine_base()
    {}

    bool force_unwind() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_unwind_stack); }

    bool preserve_fpu() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_preserve_fpu); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

    friend inline void intrusive_ptr_add_ref( pull_coroutine_base * p) BOOST_NOEXCEPT
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( pull_coroutine_base * p) BOOST_NOEXCEPT
    { if ( --p->use_count_ == 0) p->deallocate_object(); }

    void pull()
    {
        BOOST_ASSERT( ! is_complete() );

        holder< void > hldr_to( & caller_);
        holder< R > * hldr_from(
            reinterpret_cast< holder< R > * >(
                hldr_to.ctx->jump(
                    callee_,
                    reinterpret_cast< intptr_t >( & hldr_to),
                    preserve_fpu() ) ) );
        BOOST_ASSERT( hldr_from->ctx);
        callee_ = * hldr_from->ctx;
        result_ = hldr_from->data;
        if ( hldr_from->force_unwind) throw forced_unwind();
        if ( except_) rethrow_exception( except_);
    }

    bool has_result() const
    { return result_; }

    typename param< Result >::type get() const
    {
        BOOST_ASSERT( has_result() );
   
        return result_.get(); 
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_V2_DETAIL_PULL_COROUTINE_BASE_H
