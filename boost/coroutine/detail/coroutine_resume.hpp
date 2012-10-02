
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_COROUTINE_RESUME_H
#define BOOST_CORO_DETAIL_COROUTINE_RESUME_H

#include <iterator>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/context/fcontext.hpp>
#include <boost/optional.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/range.hpp>
#include <boost/type_traits/function_traits.hpp>

#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_base.hpp>
#include <boost/coroutine/detail/flags.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Signature, typename D, typename Result, int arity >
struct coroutine_resume;

template< typename Signature, typename D >
struct coroutine_resume< Signature, D, void, 0 >
{
    D & operator()()
    {
        BOOST_ASSERT( static_cast< D * >( this)->impl_);
        BOOST_ASSERT( ! static_cast< D * >( this)->impl_->is_complete() );

        context::fcontext_t caller;
        static_cast< D * >( this)->impl_->callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            static_cast< D * >( this)->impl_->callee_,
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->impl_->preserve_fpu_);
        if ( static_cast< D * >( this)->impl_->except_)
            rethrow_exception( static_cast< D * >( this)->impl_->except_);

        return * static_cast< D * >( this);
    }
};

template< typename Signature, typename D, typename Result >
struct coroutine_resume< Signature, D, Result, 0 >
{
    class iterator : public std::iterator< std::input_iterator_tag, Result >
    {
    private:
        D               *   dp_;
        optional< Result >  val_;

        void increment_()
        {
            BOOST_ASSERT( dp_);

            if ( * dp_)
            { val_ = ( * dp_)().get(); }
            else
            {
                dp_ = 0;
                val_ = none;
            }
        }

    public:
        iterator() :
            dp_( 0), val_()
        {}

        explicit iterator( D * dp) :
            dp_( dp), val_()
        { increment_(); }

        iterator( iterator const& other) :
            dp_( other.dp_), val_( other.val_)
        {}

        iterator & operator=( iterator const& other)
        {
            if ( this == & other) return * this;
            dp_ = other.dp_;
            val_ = other.val_;
            return * this;
        }

        bool operator==( iterator const& other)
        { return other.dp_ == dp_ && other.val_ == val_; }

        bool operator!=( iterator const& other)
        { return other.dp_ != dp_ || other.val_ != val_; }

        iterator & operator++()
        {
            increment_();
            return * this;
        }

        Result & operator*() const
        { return const_cast< optional< Result > & >( val_).get(); }

        Result * operator->() const
        { return const_cast< optional< Result > & >( val_).get_ptr(); }
    };

    class const_iterator : public std::iterator< std::input_iterator_tag, const Result >
    {
    private:
        D                       *   dp_;
        optional< const Result >    val_;

        void increment_()
        {
            BOOST_ASSERT( dp_);

            if ( * dp_)
            { val_ = ( * dp_)().get(); }
            else
            {
                dp_ = 0;
                val_ = none;
            }
        }

    public:
        const_iterator() :
            dp_( 0), val_()
        {}

        explicit const_iterator( D * dp) :
            dp_( dp), val_()
        { increment_(); }

        const_iterator( const_iterator const& other) :
            dp_( other.dp_), val_( other.val_)
        {}

        const_iterator & operator=( const_iterator const& other)
        {
            if ( this == & other) return * this;
            dp_ = other.dp_;
            val_ = other.val_;
            return * this;
        }

        bool operator==( const_iterator const& other)
        { return other.dp_ == dp_ && other.val_ == val_; }

        bool operator!=( const_iterator const& other)
        { return other.dp_ != dp_ || other.val_ != val_; }

        const_iterator & operator++()
        {
            increment_();
            return * this;
        }

        Result const& operator*() const
        { return val_.get(); }

        Result const* operator->() const
        { return val_.get_ptr(); }
    };

    D & operator()()
    {
        BOOST_ASSERT( static_cast< D * >( this)->impl_);
        BOOST_ASSERT( ! static_cast< D * >( this)->impl_->is_complete() );

        context::fcontext_t caller;
        static_cast< D * >( this)->impl_->callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            static_cast< D * >( this)->impl_->callee_,
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->impl_->preserve_fpu_);
        if ( static_cast< D * >( this)->impl_->except_)
            rethrow_exception( static_cast< D * >( this)->impl_->except_);

        return * static_cast< D * >( this);
    }

    Result get() const
    {
        BOOST_ASSERT( static_cast< D const* >( this)->impl_);
        BOOST_ASSERT( static_cast< D const* >( this)->impl_->result_);

        return * static_cast< D const* >( this)->impl_->result_;
    }
};

template< typename Signature, typename D >
struct coroutine_resume< Signature, D, void, 1 >
{
    class iterator : public std::iterator< std::output_iterator_tag, void, void, void, void >
    {
    private:
       D    *   dp_;

    public:
        iterator() :
           dp_( 0)
        {}

        explicit iterator( D * dp) :
            dp_( dp)
        {}

        iterator & operator=( typename function_traits< Signature >::arg1_type a1)
        {
            BOOST_ASSERT( dp_);
            if ( ! ( * dp_)( a1) ) dp_ = 0;
            return * this;
        }

        bool operator==( iterator const& other)
        { return other.dp_ == dp_; }

        bool operator!=( iterator const& other)
        { return other.dp_ != dp_; }

        iterator & operator*()
        { return * this; }

        iterator & operator++()
        { return * this; }
    };

    struct const_iterator;

    D & operator()( typename function_traits< Signature >::arg1_type a1)
    {
        BOOST_ASSERT( static_cast< D * >( this)->impl_);
        BOOST_ASSERT( ! static_cast< D * >( this)->impl_->is_complete() );

        static_cast< D * >( this)->impl_->args_ = a1;
        context::fcontext_t caller;
        static_cast< D * >( this)->impl_->callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            static_cast< D * >( this)->impl_->callee_,
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->impl_->preserve_fpu_);
        if ( static_cast< D * >( this)->impl_->except_)
            rethrow_exception( static_cast< D * >( this)->impl_->except_);

        return * static_cast< D * >( this);
    }
};

template< typename Signature, typename D, typename Result >
struct coroutine_resume< Signature, D, Result, 1 >
{
    D & operator()( typename function_traits< Signature >::arg1_type a1)
    {
        BOOST_ASSERT( static_cast< D * >( this)->impl_);
        BOOST_ASSERT( ! static_cast< D * >( this)->impl_->is_complete() );

        static_cast< D * >( this)->impl_->args_ = a1;
        context::fcontext_t caller;
        static_cast< D * >( this)->impl_->callee_ = ( context::fcontext_t *) context::jump_fcontext(
            & caller,
            static_cast< D * >( this)->impl_->callee_,
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->impl_->preserve_fpu_);
        if ( static_cast< D * >( this)->impl_->except_)
            rethrow_exception( static_cast< D * >( this)->impl_->except_);

        return * static_cast< D * >( this);
    }

    Result get() const
    {
        BOOST_ASSERT( static_cast< D const* >( this)->impl_);
        BOOST_ASSERT( static_cast< D const* >( this)->impl_->result_);

        return * static_cast< D const* >( this)->impl_->result_;
    }
};

#define BOOST_COROUTINE_RESUME_COMMA(n) BOOST_PP_COMMA_IF(BOOST_PP_SUB(n,1))
#define BOOST_COROUTINE_RESUME_VAL(z,n,unused) BOOST_COROUTINE_RESUME_COMMA(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_RESUME_VALS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_RESUME_VAL,~)
#define BOOST_COROUTINE_RESUME_ARG_TYPE(n) \
    typename function_traits< Signature >::BOOST_PP_CAT(BOOST_PP_CAT(arg,n),_type)
#define BOOST_COROUTINE_RESUME_ARG(z,n,unused) BOOST_COROUTINE_RESUME_COMMA(n) BOOST_COROUTINE_RESUME_ARG_TYPE(n) BOOST_PP_CAT(a,n)
#define BOOST_COROUTINE_RESUME_ARGS(n) BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_ADD(n,1),BOOST_COROUTINE_RESUME_ARG,~)
#define BOOST_COROUTINE_RESUME(z,n,unused) \
template< typename Signature, typename D > \
struct coroutine_resume< Signature, D, void, n > \
{ \
    D & operator()( BOOST_COROUTINE_RESUME_ARGS(n)) \
    { \
        BOOST_ASSERT( static_cast< D * >( this)->impl_); \
        BOOST_ASSERT( ! static_cast< D * >( this)->impl_->is_complete() ); \
\
        static_cast< D * >( this)->impl_->args_ = typename arg< Signature >::type_t(BOOST_COROUTINE_RESUME_VALS(n)); \
        context::fcontext_t caller; \
        static_cast< D * >( this)->impl_->callee_ = ( context::fcontext_t *) context::jump_fcontext( \
            & caller, \
            static_cast< D * >( this)->impl_->callee_, \
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->impl_->preserve_fpu_); \
        if ( static_cast< D * >( this)->impl_->except_) \
            rethrow_exception( static_cast< D * >( this)->impl_->except_); \
\
        return * static_cast< D * >( this); \
    } \
}; \
\
template< typename Signature, typename D, typename Result > \
struct coroutine_resume< Signature, D, Result, n > \
{ \
    D & operator()( BOOST_COROUTINE_RESUME_ARGS(n)) \
    { \
        BOOST_ASSERT( static_cast< D * >( this)->impl_); \
        BOOST_ASSERT( ! static_cast< D * >( this)->impl_->is_complete() ); \
\
        static_cast< D * >( this)->impl_->args_ = typename arg< Signature >::type_t(BOOST_COROUTINE_RESUME_VALS(n)); \
        context::fcontext_t caller; \
        static_cast< D * >( this)->impl_->callee_ = ( context::fcontext_t *) context::jump_fcontext( \
            & caller, \
            static_cast< D * >( this)->impl_->callee_, \
            ( intptr_t) & caller, fpu_preserved == static_cast< D * >( this)->impl_->preserve_fpu_); \
        if ( static_cast< D * >( this)->impl_->except_) \
            rethrow_exception( static_cast< D * >( this)->impl_->except_); \
\
        return * static_cast< D * >( this); \
    } \
\
    Result get() const \
    { \
        BOOST_ASSERT( static_cast< D const* >( this)->impl_); \
        BOOST_ASSERT( static_cast< D const* >( this)->impl_->result_); \
\
        return * static_cast< D const* >( this)->impl_->result_; \
    } \
};
BOOST_PP_REPEAT_FROM_TO(2,11,BOOST_COROUTINE_RESUME,~)
#undef BOOST_COROUTINE_RESUME
#undef BOOST_COROUTINE_RESUME_ARGS
#undef BOOST_COROUTINE_RESUME_ARG
#undef BOOST_COROUTINE_RESUME_ARG_TYPE
#undef BOOST_COROUTINE_RESUME_VALS
#undef BOOST_COROUTINE_RESUME_VAL
#undef BOOST_COROUTINE_RESUME_COMMA

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_COROUTINE_RESUME_H
