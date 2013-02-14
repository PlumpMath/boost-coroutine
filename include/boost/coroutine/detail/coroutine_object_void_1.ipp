
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller
>
class coroutine_object< Signature, Fn, StackAllocator, Allocator, Caller, void, 1 > :
    private stack_tuple< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, 1
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef stack_tuple< StackAllocator >                pbase_type;
    typedef coroutine_base< Signature >                 base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        coroutine_context local_ctx;
        tuple< coroutine_context const*, coroutine_object * > tpl(
            & local_ctx, this);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                local_ctx.jump(
                    this->ctx_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->ctx_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        coroutine_context local_ctx;
        tuple< coroutine_context const*, coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( & local_ctx, this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                local_ctx.jump(
                    this->ctx_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->ctx_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        coroutine_context ctx;
        coroutine_context local_ctx;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            ctx = c.impl_->ctx_;
            holder< void > hldr_to( & local_ctx);
            local_ctx.jump(
                ctx,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        ctx = c.impl_->ctx_;
        holder< void > hldr_to( & local_ctx);
        local_ctx.jump(
            ctx,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        coroutine_context local_ctx;
        holder< arg_type > hldr_to( & local_ctx, true);
        local_ctx.jump(
            this->ctx_,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( forward< Fn >( fn) ),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( forward< Fn >( fn) ),
        alloc_( alloc)
    { enter_( arg); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }
#endif

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->force_unwind() )
            unwind_stack_();
    }

    void run( coroutine_context const& ctx)
    {
        Caller c( ctx, false, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( coroutine_context const& ctx, typename detail::param< arg_type >::type arg)
    {
        Caller c( ctx, false, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller
>
class coroutine_object< Signature, reference_wrapper< Fn >, StackAllocator, Allocator, Caller, void, 1 > :
    private stack_tuple< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, 1
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef stack_tuple< StackAllocator >                pbase_type;
    typedef coroutine_base< Signature >                 base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        coroutine_context local_ctx;
        tuple< coroutine_context const*, coroutine_object * > tpl(
            & local_ctx, this);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                local_ctx.jump(
                    this->ctx_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->ctx_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        coroutine_context local_ctx;
        tuple< coroutine_context const*, coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( & local_ctx, this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                local_ctx.jump(
                    this->ctx_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->ctx_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        coroutine_context ctx;
        coroutine_context local_ctx;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            ctx = c.impl_->ctx_;
            holder< void > hldr_to( & local_ctx);
            local_ctx.jump(
                ctx,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        ctx = c.impl_->ctx_;
        holder< void > hldr_to( & local_ctx);
        local_ctx.jump(
            ctx,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        coroutine_context local_ctx;
        holder< arg_type > hldr_to( & local_ctx, true);
        local_ctx.jump(
            this->ctx_,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
    coroutine_object( reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( reference_wrapper< Fn > fn,
                      typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->force_unwind() )
            unwind_stack_();
    }

    void run( coroutine_context const& ctx)
    {
        Caller c( ctx, false, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( coroutine_context const& ctx, typename detail::param< arg_type >::type arg)
    {
        Caller c( ctx, false, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller
>
class coroutine_object< Signature, const reference_wrapper< Fn >, StackAllocator, Allocator, Caller, void, 1 > :
    private stack_tuple< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, 1
        >
    >::other                                            allocator_t;
    typedef typename arg< Signature >::type             arg_type;

private:
    typedef stack_tuple< StackAllocator >                pbase_type;
    typedef coroutine_base< Signature >                 base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object &);
    coroutine_object & operator=( coroutine_object const&);

    void enter_()
    {
        coroutine_context local_ctx;
        tuple< coroutine_context const*, coroutine_object * > tpl(
            & local_ctx, this);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                local_ctx.jump(
                    this->ctx_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->ctx_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        coroutine_context local_ctx;
        tuple< coroutine_context const*, coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( & local_ctx, this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                local_ctx.jump(
                    this->ctx_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->ctx_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        coroutine_context ctx;
        coroutine_context local_ctx;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            ctx = c.impl_->ctx_;
            holder< void > hldr_to( & local_ctx);
            local_ctx.jump(
                ctx,
                reinterpret_cast< intptr_t >( & hldr_to),
                this->preserve_fpu() );
            BOOST_ASSERT_MSG( false, "coroutine is complete");
        }
        catch ( forced_unwind const&)
        {}
        catch (...)
        { this->except_ = current_exception(); }

        this->flags_ |= flag_complete;
        ctx = c.impl_->ctx_;
        holder< void > hldr_to( & local_ctx);
        local_ctx.jump(
            ctx,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        coroutine_context local_ctx;
        holder< arg_type > hldr_to( & local_ctx, true);
        local_ctx.jump(
            this->ctx_,
            reinterpret_cast< intptr_t >( & hldr_to),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
    coroutine_object( const reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( const reference_wrapper< Fn > fn,
                      typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        pbase_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            & this->stack_ctx,
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }

    ~coroutine_object()
    {
        if ( ! this->is_complete() && this->force_unwind() )
            unwind_stack_();
    }

    void run( coroutine_context const& ctx)
    {
        Caller c( ctx, false, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( coroutine_context const& ctx, typename detail::param< arg_type >::type arg)
    {
        Caller c( ctx, false, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};
