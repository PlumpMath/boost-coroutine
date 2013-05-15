
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_COROUTINES_V2_PULL_COROUTINE_H
#define BOOST_COROUTINES_V2_PULL_COROUTINE_H

#include <cstddef>
#include <iterator>
#include <memory>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/move/move.hpp>
#include <boost/optional.hpp>
#include <boost/range.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/coroutine_context.hpp>
#include <boost/coroutine/detail/param.hpp>
#include <boost/coroutine/stack_allocator.hpp>
#include <boost/coroutine/v2/pull_coroutine_base.hpp.hpp>
#include <boost/coroutine/v2/pull_coroutine_object.hpp.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coroutines {

template< typename R >
class pull_coroutine
{
private:
    typedef detail::pull_coroutine_base< R >    base_t;
    typedef typename base_t::ptr_t              ptr_t;

    struct dummy
    { void nonnull() {} };

    typedef void ( dummy::*safe_bool)();

    ptr_t  impl_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( pull_coroutine)

public:
    pull_coroutine() BOOST_NOEXCEPT :
        impl_()
    {}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    template< typename Fn >
    explicit pull_coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
               stack_allocator const& stack_alloc =
                    stack_allocator(),
               std::allocator< pull_coroutine > const& alloc =
                    std::allocator< pull_coroutine >(),
               typename disable_if<
                   is_same< typename decay< Fn >::type, pull_coroutine >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, stack_allocator, std::allocator< pull_coroutine >
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( forward< Fn >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    explicit pull_coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< pull_coroutine > const& alloc =
                    std::allocator< pull_coroutine >(),
               typename disable_if<
                   is_same< typename decay< Fn >::type, pull_coroutine >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, StackAllocator, std::allocator< pull_coroutine >
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( forward< Fn >( fn), attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    explicit pull_coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc,
               typename disable_if<
                   is_same< typename decay< Fn >::type, pull_coroutine >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, StackAllocator, Allocator
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( forward< Fn >( fn), attr, stack_alloc, a) );
    }
#else
    template< typename Fn >
    explicit pull_coroutine( Fn fn, attributes const& attr = attributes(),
               stack_allocator const& stack_alloc =
                    stack_allocator(),
               std::allocator< pull_coroutine > const& alloc =
                    std::allocator< pull_coroutine >(),
               typename disable_if<
                   is_convertible< Fn &, BOOST_RV_REF( Fn) >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, stack_allocator, std::allocator< pull_coroutine >
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    explicit pull_coroutine( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< pull_coroutine > const& alloc =
                    std::allocator< pull_coroutine >(),
               typename disable_if<
                   is_convertible< Fn &, BOOST_RV_REF( Fn) >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, StackAllocator, std::allocator< pull_coroutine >
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    explicit pull_coroutine( Fn fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc,
               typename disable_if<
                   is_convertible< Fn &, BOOST_RV_REF( Fn) >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, StackAllocator, Allocator
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn >
    explicit pull_coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr = attributes(),
               stack_allocator const& stack_alloc =
                    stack_allocator(),
               std::allocator< pull_coroutine > const& alloc =
                    std::allocator< pull_coroutine >(),
               typename disable_if<
                   is_same< typename decay< Fn >::type, pull_coroutine >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, stack_allocator, std::allocator< pull_coroutine >
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator >
    explicit pull_coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               std::allocator< pull_coroutine > const& alloc =
                    std::allocator< pull_coroutine >(),
               typename disable_if<
                   is_same< typename decay< Fn >::type, pull_coroutine >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, StackAllocator, std::allocator< pull_coroutine >
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }

    template< typename Fn, typename StackAllocator, typename Allocator >
    explicit pull_coroutine( BOOST_RV_REF( Fn) fn, attributes const& attr,
               StackAllocator const& stack_alloc,
               Allocator const& alloc,
               typename disable_if<
                   is_same< typename decay< Fn >::type, pull_coroutine >,
                   dummy *
               >::type = 0) :
        impl_()
    {
        typedef detail::pull_coroutine_object<
                R, Fn, StackAllocator, Allocator
            >                               object_t;
        typename object_t::allocator_t a( alloc);
        impl_ = ptr_t(
            // placement new
            ::new( a.allocate( 1) ) object_t( fn, attr, stack_alloc, a) );
    }
#endif

    pull_coroutine( BOOST_RV_REF( pull_coroutine) other) BOOST_NOEXCEPT :
        impl_()
    { swap( other); }

    pull_coroutine & operator=( BOOST_RV_REF( pull_coroutine) other) BOOST_NOEXCEPT
    {
        pull_coroutine tmp( boost::move( other) );
        swap( tmp);
        return * this;
    }

    bool empty() const BOOST_NOEXCEPT
    { return ! impl_; }

    operator safe_bool() const BOOST_NOEXCEPT
    { return ( empty() || impl_->is_complete() ) ? 0 : & dummy::nonnull; }

    bool operator!() const BOOST_NOEXCEPT
    { return empty() || impl_->is_complete(); }

    void swap( pull_coroutine & other) BOOST_NOEXCEPT
    { impl_.swap( other.impl_); }

    void operator()()
    {
        BOOST_ASSERT( * this);

        impl_->pull();
    }

    bool has_result() const
    {
        BOOST_ASSERT( * this);
   
        return impl_->has_result(); 
    }

    typename param< Result >::type get() const
    {
        BOOST_ASSERT( * this);
        BOOST_ASSERT( has_result() );
   
        return impl_->get(); 
    }

    class iterator : public std::iterator< std::input_iterator_tag, typename remove_reference< R >::type >
    {
    private:
        pull_coroutine< R > *   c_;
        optional< R >           val_;

        void fetch_()
        {
            BOOST_ASSERT( c_);

            if ( ! c_->has_result() )
            {
                c_ = 0;
                val_ = none;
                return;
            }
            val_ = c_->get();
        }

        void increment_()
        {
            BOOST_ASSERT( c_);
            BOOST_ASSERT( * c_);

            ( * c_)();
            fetch_();
        }

    public:
        typedef typename iterator::pointer      pointer_t;
        typedef typename iterator::reference    reference_t;

        iterator() :
            c_( 0), val_()
        {}

        explicit iterator( pull_coroutine< R > * c) :
            c_( c), val_()
        { fetch_(); }

        iterator( iterator const& other) :
            c_( other.c_), val_( other.val_)
        {}

        iterator & operator=( iterator const& other)
        {
            if ( this == & other) return * this;
            c_ = other.c_;
            val_ = other.val_;
            return * this;
        }

        bool operator==( iterator const& other)
        { return other.c_ == c_ && other.val_ == val_; }

        bool operator!=( iterator const& other)
        { return other.c_ != c_ || other.val_ != val_; }

        iterator & operator++()
        {
            increment_();
            return * this;
        }

        iterator operator++( int)
        {
            iterator tmp( * this);
            ++*this;
            return tmp;
        }

        reference_t operator*() const
        { return const_cast< optional< R > & >( val_).get(); }

        pointer_t operator->() const
        { return const_cast< optional< R > & >( val_).get_ptr(); }
    };

    class const_iterator : public std::iterator< std::input_iterator_tag, const typename remove_reference< R >::type >
    {
    private:
        pull_coroutine< R > *   c_;
        optional< R >      val_;

        void fetch_()
        {
            BOOST_ASSERT( c_);

            if ( ! c_->has_result() )
            {
                c_ = 0;
                val_ = none;
                return;
            }
            val_ = c_->get();
        }

        void increment_()
        {
            BOOST_ASSERT( c_);
            BOOST_ASSERT( * c_);

            ( * c_)();
            fetch_();
        }

    public:
        typedef typename const_iterator::pointer      pointer_t;
        typedef typename const_iterator::reference    reference_t;

        const_iterator() :
            c_( 0), val_()
        {}

        explicit const_iterator( pull_coroutine< R > const* c) :
            c_( const_cast< pull_coroutine< R > * >( c) ), val_()
        { fetch_(); }

        const_iterator( const_iterator const& other) :
            c_( other.c_), val_( other.val_)
        {}

        const_iterator & operator=( const_iterator const& other)
        {
            if ( this == & other) return * this;
            c_ = other.c_;
            val_ = other.val_;
            return * this;
        }

        bool operator==( const_iterator const& other)
        { return other.c_ == c_ && other.val_ == val_; }

        bool operator!=( const_iterator const& other)
        { return other.c_ != c_ || other.val_ != val_; }

        const_iterator & operator++()
        {
            increment_();
            return * this;
        }

        const_iterator operator++( int)
        {
            const_iterator tmp( * this);
            ++*this;
            return tmp;
        }

        reference_t operator*() const
        { return val_.get(); }

        pointer_t operator->() const
        { return val_.get_ptr(); }
    };
};

template< typename R >
void swap( pull_coroutine< R > & l, pull_coroutine< R > & r) BOOST_NOEXCEPT
{ l.swap( r); }

template< typename R >
inline
typename pull_coroutine< R >::iterator
range_begin( pull_coroutine< R > & c)
{ return typename pull_coroutine< R >::iterator( & c); }

template< typename R >
inline
typename pull_coroutine< R >::const_iterator
range_begin( pull_coroutine< R > const& c)
{ return typename pull_coroutine< R >::const_iterator( & c); }

template< typename R >
inline
typename pull_coroutine< R >::iterator
range_end( pull_coroutine< R > &)
{ return typename pull_coroutine< R >::iterator(); }

template< typename R >
inline
typename pull_coroutine< R >::const_iterator
range_end( pull_coroutine< R > const&)
{ return typename pull_coroutine< R >::const_iterator(); }

template< typename R >
inline
typename pull_coroutine< R >::iterator
begin( pull_coroutine< R > & c)
{ return boost::begin( c); }

template< typename R >
inline
typename pull_coroutine< R >::iterator
end( pull_coroutine< R > & c)
{ return boost::end( c); }

template< typename R >
inline
typename pull_coroutine< R >::const_iterator
begin( pull_coroutine< R > const& c)
{ return boost::const_begin( c); }

template< typename R >
inline
typename pull_coroutine< R >::const_iterator
end( pull_coroutine< R > const& c)
{ return boost::const_end( c); }

}

template< typename R >
struct range_mutable_iterator< coroutines::pull_coroutine< R > >
{ typedef typename coroutines::pull_coroutine< R >::iterator type; };

template< typename R >
struct range_const_iterator< coroutines::pull_coroutine< R > >
{ typedef typename coroutines::pull_coroutine< R >::const_iterator type; };

}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_COROUTINES_V2_PULL_COROUTINE_H
