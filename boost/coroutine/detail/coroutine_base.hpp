
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_BASE_H
#define BOOST_CORO_DETAIL_COROUTINE_BASE_H

#if defined(__PGI)
#include <stdint.h>
#endif

#include <cstddef>
#include <utility>

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

template< typename Signature, typename Result, int arity >
class coroutine_base;

template< typename Signature >
class coroutine_base< Signature, void, 0 > : private noncopyable,
                       public coroutine_base_resume<
                            Signature,
                            coroutine_base< Signature, void, 0 >,
                            void, 0
                       >
{
public:
    typedef intrusive_ptr< coroutine_base >      ptr_t;

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_base_resume;
    template< typename X, typename Y, typename Z, typename A, typename B, int, typename C >
    friend class coroutine_object;

    std::size_t             use_count_;
    std::size_t             size_;
    void                *   sp_;
    context::fcontext_t     caller_;
    context::fcontext_t *   callee_;
    int                     flags_;
    exception_ptr           except_;

protected:
    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator >
    coroutine_base( void ( * fn)( intptr_t), attributes const& attr, StackAllocator const& alloc) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, void, 0 >,
            void, 0
        >(),
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        caller_(),
        callee_( context::make_fcontext( sp_, size_, fn) ),
        flags_( 0),
        except_()
    {
        if ( stack_unwind == attr.do_unwind) flags_ |= flag_force_unwind;
        if ( attr.preserve_fpu) flags_ |= flag_preserve_fpu;
    }

    coroutine_base( context::fcontext_t * callee, bool preserve_fpu) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, void, 0 >,
            void, 0
        >(),
        use_count_( 0),
        size_( 0),
        sp_( 0),
        caller_(),
        callee_( callee),
        flags_( 0),
        except_()
    { if ( preserve_fpu) flags_ |= flag_preserve_fpu; }

    virtual ~coroutine_base()
    {}

    bool force_unwind() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_unwind_stack); }

    bool preserve_fpu() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_preserve_fpu); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

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
                            Result, 0
                       >
{
public:
    typedef intrusive_ptr< coroutine_base >      ptr_t;

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_base_resume;
    template< typename X, typename Y, typename Z, typename A, typename B, int, typename C >
    friend class coroutine_object;

    std::size_t             use_count_;
    std::size_t             size_;
    void                *   sp_;
    context::fcontext_t     caller_;
    context::fcontext_t *   callee_;
    int                     flags_;
    exception_ptr           except_;

protected:
    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator >
    coroutine_base( void ( * fn)( intptr_t), attributes const& attr, StackAllocator const& alloc) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, Result, 0 >,
            Result, 0
        >(),
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        caller_(),
        callee_( context::make_fcontext( sp_, size_, fn) ),
        flags_( 0),
        except_()
    {
        if ( stack_unwind == attr.do_unwind) flags_ |= flag_force_unwind;
        if ( attr.preserve_fpu) flags_ |= flag_preserve_fpu;
    }

    coroutine_base( context::fcontext_t * callee, bool preserve_fpu) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, Result, 0 >,
            Result, 0
        >(),
        use_count_( 0),
        size_( 0),
        sp_( 0),
        caller_(),
        callee_( callee),
        flags_( 0),
        except_()
    { if ( preserve_fpu) flags_ |= flag_preserve_fpu; }

    virtual ~coroutine_base()
    {}

    bool force_unwind() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool preserve_fpu() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_preserve_fpu); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

    friend inline void intrusive_ptr_add_ref( coroutine_base * p) BOOST_NOEXCEPT
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( coroutine_base * p) BOOST_NOEXCEPT
    { if ( --p->use_count_ == 0) p->deallocate_object(); }
};

template< typename Signature, int arity >
class coroutine_base< Signature, void, arity > : private noncopyable,
                       public coroutine_base_resume<
                            Signature,
                            coroutine_base< Signature, void, arity >,
                            void, arity
                       >
{
public:
    typedef intrusive_ptr< coroutine_base >     ptr_t;
    typedef typename arg< Signature >::type     arg_type;

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_base_resume;
    template< typename X, typename Y, typename Z, typename A, typename B, int, typename C >
    friend class coroutine_object;

    std::size_t             use_count_;
    std::size_t             size_;
    void                *   sp_;
    context::fcontext_t     caller_;
    context::fcontext_t *   callee_;
    int                     flags_;
    exception_ptr           except_;

protected:
    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator >
    coroutine_base( void ( * fn)( intptr_t), attributes const& attr, StackAllocator const& alloc) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, void, arity >,
            void, arity
        >(),
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        caller_(),
        callee_( context::make_fcontext( sp_, size_, fn) ),
        flags_( 0),
        except_()
    {
        if ( stack_unwind == attr.do_unwind) flags_ |= flag_force_unwind;
        if ( attr.preserve_fpu) flags_ |= flag_preserve_fpu;
    }

    coroutine_base( context::fcontext_t * callee, bool preserve_fpu) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, void, arity >,
            void, arity
        >(),
        use_count_( 0),
        size_( 0),
        sp_( 0),
        caller_(),
        callee_( callee),
        flags_( 0),
        except_()
    { if ( preserve_fpu) flags_ |= flag_preserve_fpu; }

    virtual ~coroutine_base()
    {}

    bool force_unwind() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_unwind_stack); }

    bool preserve_fpu() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_preserve_fpu); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

    friend inline void intrusive_ptr_add_ref( coroutine_base * p) BOOST_NOEXCEPT
    { ++p->use_count_; }

    friend inline void intrusive_ptr_release( coroutine_base * p) BOOST_NOEXCEPT
    { if ( --p->use_count_ == 0) p->deallocate_object(); }
};

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
    typedef intrusive_ptr< coroutine_base >     ptr_t;
    typedef typename arg< Signature >::type     arg_type;

private:
    template< typename X, typename Y, typename Z, int >
    friend struct coroutine_base_resume;
    template< typename X, typename Y, typename Z, typename A, typename B, int, typename C >
    friend class coroutine_object;

    std::size_t             use_count_;
    std::size_t             size_;
    void                *   sp_;
    context::fcontext_t     caller_;
    context::fcontext_t *   callee_;
    int                     flags_;
    exception_ptr           except_;

protected:
    virtual void deallocate_object() = 0;

public:
    template< typename StackAllocator >
    coroutine_base( void ( * fn)( intptr_t), attributes const& attr, StackAllocator const& alloc) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, Result, arity >,
            Result, arity
        >(),
        use_count_( 0),
        size_( attr.size),
        sp_( alloc.allocate( size_) ),
        caller_(),
        callee_( context::make_fcontext( sp_, size_, fn) ),
        flags_( 0),
        except_()
    {
        if ( stack_unwind == attr.do_unwind) flags_ |= flag_force_unwind;
        if ( attr.preserve_fpu) flags_ |= flag_preserve_fpu;
    }

    coroutine_base( context::fcontext_t * callee, bool preserve_fpu) :
        coroutine_base_resume<
            Signature,
            coroutine_base< Signature, Result, arity >,
            Result, arity
        >(),
        use_count_( 0),
        size_( 0),
        sp_( 0),
        caller_(),
        callee_( callee),
        flags_( 0),
        except_()
    { if ( preserve_fpu) flags_ |= flag_preserve_fpu; }

    virtual ~coroutine_base()
    {}

    bool force_unwind() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_force_unwind); }

    bool unwind_requested() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_unwind_stack); }

    bool preserve_fpu() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_preserve_fpu); }

    bool is_complete() const BOOST_NOEXCEPT
    { return 0 != ( flags_ & flag_complete); }

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
