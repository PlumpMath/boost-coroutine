
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_OBJECT_H
#define BOOST_CORO_DETAIL_COROUTINE_OBJECT_H

#include <cstddef>

#include <boost/config.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/move/move.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits/add_pointer.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/param.hpp>

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

    holder< Context * > * hldr(
        reinterpret_cast< holder< Context * > * >( vp) );
    Context * ctx( hldr->data.get() );

    ctx->run( hldr->ctx);
}

template< typename Context, typename Arg >
void trampoline2( intptr_t vp)
{
    BOOST_ASSERT( vp);

    holder< tuple< Context *, Arg > > * hldr(
        reinterpret_cast< holder< tuple< Context *, Arg > > * >( vp) );
    Context * ctx( hldr->data.get().get< 0 >() );
    Arg arg( hldr->data.get().get< 1 >() );

    ctx->run( hldr->ctx, arg);
}

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Result, int arity, typename Caller
>
class coroutine_object;

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Caller
>
class coroutine_object< Fn, StackAllocator, Allocator, Signature, void, 0, Caller > :
    public coroutine_base< Signature, void, 0 >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, void, 0, Caller
        >
    >::other                                            allocator_t;

private:
    typedef coroutine_base< Signature, void, 0 >    base_type;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object const&);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< coroutine_object * > hldr_to( & this->caller_, this);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    coroutine_object( Fn && fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->unwind_forced() ) this->unwind_stack();
        stack_alloc_.deallocate( this->sp_, this->size_);
    }

    void run( context::fcontext_t * callee)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        context::fcontext_t caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            BOOST_ASSERT( callee);
            holder< void > hldr_to( & caller);
            context::jump_fcontext(
                hldr_to.ctx, callee,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        BOOST_ASSERT( callee);
        context::jump_fcontext(
            & caller, callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Result, typename Caller
>
class coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, 0, Caller > :
    public coroutine_base< Signature, Result, 0 >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, Result, 0, Caller
        >
    >::other                                            allocator_t;

private:
    typedef coroutine_base< Signature, Result, 0 >  base_type;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object const&);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< coroutine_object * > hldr_to( & this->caller_, this);
        holder< Result > * hldr_from(
            reinterpret_cast< holder< Result > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        this->result_ = hldr_from->data;
        if ( this->except_) rethrow_exception( this->except_);
    }

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    coroutine_object( Fn && fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->unwind_forced() ) this->unwind_stack();
        stack_alloc_.deallocate( this->sp_, this->size_);
    }

    void run( context::fcontext_t * callee)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        context::fcontext_t caller;
        try
        {
            fn_( c);
            holder< Result > hldr_to( & caller);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            BOOST_ASSERT( callee);
            context::jump_fcontext(
                hldr_to.ctx, callee,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        BOOST_ASSERT( callee);
        context::jump_fcontext(
            & caller, callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Caller
>
class coroutine_object< Fn, StackAllocator, Allocator, Signature, void, 1, Caller > :
    public coroutine_base< Signature, void, 1 >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, void, 1, Caller
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef coroutine_base< Signature, void, 1 >        base_type;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< coroutine_object * > hldr_to( & this->caller_, this);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        holder< tuple< coroutine_object *, typename detail::param< arg_type >::type > > hldr_to(
            & this->caller_, tuple< coroutine_object *, typename detail::param< arg_type >::type >( this, arg) );
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        context::fcontext_t * callee( 0);
        context::fcontext_t caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            BOOST_ASSERT( callee);
            holder< void > hldr( & caller);
            context::jump_fcontext(
                hldr.ctx, callee,
                ( intptr_t) & hldr,
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        BOOST_ASSERT( callee);
        context::jump_fcontext(
            & caller, callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    coroutine_object( Fn && fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn && fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->unwind_forced() ) this->unwind_stack();
        stack_alloc_.deallocate( this->sp_, this->size_);
    }

    void run( context::fcontext_t * callee)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( context::fcontext_t * callee, typename detail::param< arg_type >::type arg)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Result, typename Caller
>
class coroutine_object< Fn, StackAllocator, Allocator, Signature, Result, 1, Caller > :
    public coroutine_base< Signature, Result, 1 >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, Result, 1, Caller
        >
    >::other                                          allocator_t;
    typedef typename arg< Signature >::type           arg_type;

private:
    typedef coroutine_base< Signature, Result, 1 >    base_type;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< coroutine_object * > hldr_to( & this->caller_, this);
        holder< Result > * hldr_from(
            reinterpret_cast< holder< Result > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        this->result_ = hldr_from->data;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        holder<
            tuple< coroutine_object *,
            typename detail::param< arg_type >::type >
        > hldr_to(
            & this->caller_, tuple< coroutine_object *,
            typename detail::param< arg_type >::type >( this, arg) );
        holder< Result > * hldr_from(
            reinterpret_cast< holder< Result > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        this->result_ = hldr_from->data;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        context::fcontext_t * callee( 0);
        context::fcontext_t caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            BOOST_ASSERT( callee);
            holder< Result > hldr_to( & caller);
            context::jump_fcontext(
                hldr_to.ctx, callee,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        BOOST_ASSERT( callee);
        context::jump_fcontext(
            & caller, callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    coroutine_object( Fn && fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn && fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->unwind_forced() ) this->unwind_stack();
        stack_alloc_.deallocate( this->sp_, this->size_);
    }

    void run( context::fcontext_t * callee)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( context::fcontext_t * callee, typename detail::param< arg_type >::type arg)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, int arity, typename Caller
>
class coroutine_object< Fn, StackAllocator, Allocator, Signature, void, arity, Caller > :
    public coroutine_base< Signature, void, arity >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, void, arity, Caller
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef coroutine_base< Signature, void, arity >    base_type;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< coroutine_object * > hldr_to( & this->caller_, this);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        holder<
            tuple< coroutine_object *,
            typename detail::param< arg_type >::type >
        > hldr_to(
            & this->caller_, tuple< coroutine_object *,
            typename detail::param< arg_type >::type >( this, arg) );
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        context::fcontext_t * callee( 0);
        context::fcontext_t caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            BOOST_ASSERT( callee);
            holder< void > hldr( & caller);
            context::jump_fcontext(
                hldr.ctx, callee,
                ( intptr_t) & hldr,
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        BOOST_ASSERT( callee);
        context::jump_fcontext(
            & caller, callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    coroutine_object( Fn && fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn && fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->unwind_forced() ) this->unwind_stack();
        stack_alloc_.deallocate( this->sp_, this->size_);
    }

    void run( context::fcontext_t * callee)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( context::fcontext_t * callee, typename detail::param< arg_type >::type arg)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Fn, typename StackAllocator, typename Allocator,
    typename Signature, typename Result, int arity, typename Caller
>
class coroutine_object :
    public coroutine_base< Signature, Result, arity >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Fn, StackAllocator, Allocator, Signature, Result, arity, Caller
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef coroutine_base< Signature, Result, arity >  base_type;

    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        holder< coroutine_object * > hldr_to( & this->caller_, this);
        holder< Result > * hldr_from(
            reinterpret_cast< holder< Result > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        this->result_ = hldr_from->data;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        holder<
            tuple< coroutine_object *,
            typename detail::param< arg_type >::type >
        > hldr_to(
            & this->caller_, tuple< coroutine_object *,
            typename detail::param< arg_type >::type >( this, arg) );
        holder< Result > * hldr_from(
            reinterpret_cast< holder< Result > * >( context::jump_fcontext(
                hldr_to.ctx, this->callee_,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() ) ) );
        this->callee_ = hldr_from->ctx;
        this->result_ = hldr_from->data;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        context::fcontext_t * callee( 0);
        context::fcontext_t caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            BOOST_ASSERT( callee);
            holder< Result > hldr_to( & caller);
            context::jump_fcontext(
                hldr_to.ctx, callee,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        callee = c.impl_->callee_;
        BOOST_ASSERT( callee);
        context::jump_fcontext(
            & caller, callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    coroutine_object( Fn && fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn && fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline1< coroutine_object >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    { enter_( arg); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->unwind_forced() ) this->unwind_stack();
        stack_alloc_.deallocate( this->sp_, this->size_);
    }

    void run( context::fcontext_t * callee)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( context::fcontext_t * callee, typename detail::param< arg_type >::type arg)
    {
        Caller c( callee, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_OBJECT_H
