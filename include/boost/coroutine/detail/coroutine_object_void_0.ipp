
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller
>
class coroutine_object< Signature, Fn, StackAllocator, Allocator, Caller, void, 0 > :
    private stack_data< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, 0
        >
    >::other                                        allocator_t;

private:
    typedef stack_data< StackAllocator >            stack_type;
    typedef coroutine_base< Signature >             base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object const&);
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

    void run_( Caller & c)
    {
        controll_block callee;
        controll_block caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            holder< void > hldr_to( & caller);
            hldr_to.ctx->jump(
                callee,
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
        holder< void > hldr( & this->caller_, true);
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

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller
>
class coroutine_object< Signature, reference_wrapper< Fn >, StackAllocator, Allocator, Caller, void, 0 > :
    private stack_data< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, 0
        >
    >::other                                        allocator_t;

private:
    typedef stack_data< StackAllocator >            stack_type;
    typedef coroutine_base< Signature >             base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object const&);
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

    void run_( Caller & c)
    {
        controll_block callee;
        controll_block caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            holder< void > hldr_to( & caller);
            hldr_to.ctx->jump(
                callee,
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
        holder< void > hldr( & this->caller_, true);
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

    void deallocate_object()
    { destroy_( alloc_, this); }
};

template<
    typename Signature,
    typename Fn, typename StackAllocator, typename Allocator,
    typename Caller
>
class coroutine_object< Signature, const reference_wrapper< Fn >, StackAllocator, Allocator, Caller, void, 0 > :
    private stack_data< StackAllocator >,
    public coroutine_base< Signature >
{
public:
    typedef typename Allocator::template rebind<
        coroutine_object<
            Signature, Fn, StackAllocator, Allocator, Caller, void, 0
        >
    >::other                                        allocator_t;

private:
    typedef stack_data< StackAllocator >            stack_type;
    typedef coroutine_base< Signature >             base_type;

    Fn                      fn_;
    allocator_t             alloc_;

    static void destroy_( allocator_t & alloc, coroutine_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }

    coroutine_object( coroutine_object const&);
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

    void run_( Caller & c)
    {
        controll_block callee;
        controll_block caller;
        try
        {
            fn_( c);
            this->flags_ |= flag_complete;
            callee = c.impl_->callee_;
            holder< void > hldr_to( & caller);
            hldr_to.ctx->jump(
                callee,
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
        holder< void > hldr( & this->caller_, true);
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

    void deallocate_object()
    { destroy_( alloc_, this); }
};
