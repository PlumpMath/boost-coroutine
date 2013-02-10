
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
    private stack_data< StackAllocator >,
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
    typedef stack_data< StackAllocator >                stack_type;
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
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( this),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        tuple< coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        controll_block callee;
        controll_block caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            holder< void > hldr( & caller);
            hldr.ctx->jump(
                callee,
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
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        holder< arg_type > hldr( & this->caller_, true);
        hldr.ctx->jump(
            this->callee_,
            reinterpret_cast< intptr_t >( & hldr),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( forward< Fn >( fn) ),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( forward< Fn >( fn) ),
        alloc_( alloc)
    { enter_( arg); }
#else
    coroutine_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( Fn fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_( arg); }

    coroutine_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( BOOST_RV_REF( Fn) fn, typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
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

    void run()
    {
        Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( typename detail::param< arg_type >::type arg)
    {
        Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
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
    private stack_data< StackAllocator >,
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
    typedef stack_data< StackAllocator >                stack_type;
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
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( this),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        tuple< coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        controll_block callee;
        controll_block caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            holder< void > hldr( & caller);
            hldr.ctx->jump(
                callee,
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
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        holder< arg_type > hldr( & this->caller_, true);
        hldr.ctx->jump(
            this->callee_,
            reinterpret_cast< intptr_t >( & hldr),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
    coroutine_object( reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( reference_wrapper< Fn > fn,
                      typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
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

    void run()
    {
        Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( typename detail::param< arg_type >::type arg)
    {
        Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
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
    private stack_data< StackAllocator >,
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
    typedef stack_data< StackAllocator >                stack_type;
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
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( this),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void enter_( typename detail::param< arg_type >::type arg)
    {
        tuple< coroutine_object *,
            typename detail::param< arg_type >::type
        > tpl( this, arg);
        holder< void > * hldr_from(
            reinterpret_cast< holder< void > * >(
                this->caller_.jump(
                    this->callee_,
                    reinterpret_cast< intptr_t >( & tpl),
                    this->preserve_fpu() ) ) );
        this->callee_ = * hldr_from->ctx;
        if ( this->except_) rethrow_exception( this->except_);
    }

    void run_( Caller & c)
    {
        controll_block callee;
        controll_block caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            holder< void > hldr( & caller);
            hldr.ctx->jump(
                callee,
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
        caller.jump(
            callee,
            reinterpret_cast< intptr_t >( & caller),
            this->preserve_fpu() );
        BOOST_ASSERT_MSG( false, "coroutine is complete");
    }

    void unwind_stack_() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( ! this->is_complete() );

        this->flags_ |= flag_unwind_stack;
        holder< arg_type > hldr( & this->caller_, true);
        hldr.ctx->jump(
            this->callee_,
            reinterpret_cast< intptr_t >( & hldr),
            this->preserve_fpu() );
        this->flags_ &= ~flag_unwind_stack;

        BOOST_ASSERT( this->is_complete() );
    }

public:
    coroutine_object( const reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline1< coroutine_object >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
            stack_unwind == attr.do_unwind,
            fpu_preserved == attr.preserve_fpu),
        fn_( fn),
        alloc_( alloc)
    { enter_(); }

    coroutine_object( const reference_wrapper< Fn > fn,
                      typename detail::param< arg_type >::type arg, attributes const& attr,
                      StackAllocator const& stack_alloc,
                      allocator_t const& alloc) :
        stack_type( stack_alloc, attr.size),
        base_type(
            trampoline2< coroutine_object, typename detail::param< arg_type >::type >,
            stack_type::sp, stack_type::size,
#if defined(BOOST_USE_SEGMENTED_STACKS)
            & stack_type::seg[0],
#endif
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

    void run()
    {
        Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
        run_( c);
    }

    void run( typename detail::param< arg_type >::type arg)
    {
        Caller c( this->caller_, false, this->preserve_fpu(), alloc_);
        c.impl_->result_ = arg;
        run_( c);
    }

    void deallocate_object()
    { destroy_( alloc_, this); }
};
