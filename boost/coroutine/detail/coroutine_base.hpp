
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
#include <boost/coroutine/detail/arg.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base_resume.hpp>
#include <boost/coroutine/detail/exceptions.hpp>
#include <boost/coroutine/detail/flags.hpp>
#include <boost/coroutine/detail/holder.hpp>
#include <boost/coroutine/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Context >
void trampoline1( intptr_t vp)
{
    BOOST_ASSERT( vp);

    holder< Context * > * hldr( reinterpret_cast< holder< Context * > * >( vp) );
    Context * ctx( hldr->data.get() );

    ctx->run( hldr->ctx);
}

template< typename Context, typename Arg >
void trampoline2( intptr_t vp)
{
    BOOST_ASSERT( vp);

    holder< Context * > * hldr( reinterpret_cast< holder< Context * > * >( vp) );
    Context * ctx( hldr->data.get() );

    ctx->run( hldr->ctx);
}

template< typename Signature, typename Result, int arity >
class coroutine_base : private noncopyable,
                       public coroutine_base_resume<
                            Signature,
                            coroutine_base< Signature, Result, arity >,
                            Result,
                            arity
                       >
{
public:
    typedef intrusive_ptr< coroutine_base >      ptr_t;
    typedef typename arg< Signature >::type_t    arg_type;

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_base_resume;
    template< typename X, typename Y, typename Z, int, typename C >
    friend struct coroutine_exec;

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
        if ( sp_) alloc.deallocate( sp_, size_);
    }

    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator, typename D >
    coroutine_base( attributes const& attr, StackAllocator const& alloc,
                    D const* dummy) :
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        callee_(
            context::make_fcontext(
                sp_, size_, trampoline1< D >) ),
        flags_( stack_unwind == attr.do_unwind
            ? flag_force_unwind
            : flag_dont_force_unwind),
        except_(),
        preserve_fpu_( attr.preserve_fpu)
    {
        context::fcontext_t caller;
        holder< coroutine_base * > hldr( & caller, this);
        callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller, callee_, ( intptr_t) & hldr, false);
    }

    template< typename StackAllocator, typename D >
    coroutine_base( arg_type const& arg, attributes const& attr,
                    StackAllocator const& alloc, D const* dummy) :
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        callee_(
            context::make_fcontext(
                sp_, size_, trampoline2< D >) ),
        flags_( stack_unwind == attr.do_unwind
            ? flag_force_unwind
            : flag_dont_force_unwind),
        except_(),
        preserve_fpu_( attr.preserve_fpu)
    {
        context::fcontext_t caller;
        holder< coroutine_base * > hldr( & caller, this);
        callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller, callee_, ( intptr_t) & hldr, false);
    }

    coroutine_base( context::fcontext_t * callee, bool preserve_fpu) :
        use_count_( 0),
        size_( 0),
        sp_( 0),
        callee_( callee),
        flags_( flag_dont_force_unwind),
        except_(),
        preserve_fpu_( preserve_fpu)
    {}

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
        typedef typename arg< Signature >::type_t   arg_t;

        BOOST_ASSERT( ! is_complete() );

        flags_ |= flag_unwind_stack;
        context::fcontext_t caller;
        holder< arg_t > hldr( & caller, true);
        context::jump_fcontext(
            hldr.ctx, callee_,
            ( intptr_t) & hldr, fpu_preserved == preserve_fpu_);
        flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( is_complete() );
    }

    friend inline void intrusive_ptr_add_ref( coroutine_base * p) BOOST_NOEXCEPT
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( coroutine_base * p) BOOST_NOEXCEPT
    { if ( --p->use_count_ == 0) p->deallocate_object(); }
};

template< typename Signature, typename Result >
class coroutine_base< Signature, Result, 0 > : private noncopyable,
                       public coroutine_base_resume<
                            Signature,
                            coroutine_base< Signature, Result, 0 >,
                            Result,
                            0
                       >
{
public:
    typedef intrusive_ptr< coroutine_base >      ptr_t;

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_base_resume;
    template< typename X, typename Y, typename Z, int, typename C >
    friend struct coroutine_exec;

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
        if ( sp_) alloc.deallocate( sp_, size_);
    }

    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator, typename D >
    coroutine_base( attributes const& attr, StackAllocator const& alloc,
                    D const* dummy) :
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        callee_(
            context::make_fcontext(
                sp_, size_, trampoline1< D >) ),
        flags_( stack_unwind == attr.do_unwind
            ? flag_force_unwind
            : flag_dont_force_unwind),
        except_(),
        preserve_fpu_( attr.preserve_fpu)
    {
        context::fcontext_t caller;
        holder< coroutine_base * > hldr( & caller, this);
        callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller, callee_, ( intptr_t) & hldr, false);
    }

    coroutine_base( context::fcontext_t * callee, bool preserve_fpu) :
        use_count_( 0),
        size_( 0),
        sp_( 0),
        callee_( callee),
        flags_( flag_dont_force_unwind),
        except_(),
        preserve_fpu_( preserve_fpu)
    {}

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
        holder< void > hldr( & caller, true);
        context::jump_fcontext(
            hldr.ctx, callee_,
            ( intptr_t) & hldr, fpu_preserved == preserve_fpu_);
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
