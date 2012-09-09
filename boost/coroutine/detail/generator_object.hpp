
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_GENERATOR_OBJECT_H
#define BOOST_CORO_DETAIL_GENERATOR_OBJECT_H

#include <cstddef>

#include <boost/config.hpp>
#include <boost/move/move.hpp>
#include <boost/ref.hpp>

#include <boost/coroutine/attributes.hpp>
#include <boost/coroutine/detail/config.hpp>
#include <boost/coroutine/detail/generator_base.hpp>
#include <boost/coroutine/detail/generator_self.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Fn, typename StackAllocator, typename Allocator, typename Result >
class generator_object : public generator_base< Result >
{
public:
    typedef typename Allocator::template rebind<
        generator_object<
            Fn, StackAllocator, Allocator, Result
        >
    >::other   allocator_t;

#ifndef BOOST_NO_RVALUE_REFERENCES
    generator_object( Fn && fn, attributes const& attr,
                      StackAllocator const& stack_alloc, allocator_t const& alloc) :
        generator_base< Result >( attr, stack_alloc),
        fn_( static_cast< Fn && >( fn) ),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}
#else
    generator_object( Fn fn, attributes const& attr,
                      StackAllocator const& stack_alloc, allocator_t const& alloc) :
        generator_base< Result >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}

    generator_object( BOOST_RV_REF( Fn) fn, attributes const& attr,
                      StackAllocator const& stack_alloc, allocator_t const& alloc) :
        generator_base< Result >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}
#endif

    ~generator_object()
    { deallocate_stack( stack_alloc_); }

    void exec_()
    {
        generator_self< Result > self( this);
        fn_( self);
    }

    void deallocate_object()
    { destroy( alloc_, this); }

private:
    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    generator_object( generator_object &);
    generator_object & operator=( generator_object const&);

    static void destroy( allocator_t & alloc, generator_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

template< typename Fn, typename StackAllocator, typename Allocator, typename Result >
class generator_object< reference_wrapper< Fn >, StackAllocator, Allocator, Result > :
    public generator_base< Result >
{
public:
    typedef typename Allocator::template rebind<
        generator_object<
            Fn, StackAllocator, Allocator, Result
        >
    >::other   allocator_t;

    generator_object( reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc, allocator_t const& alloc) :
        generator_base< Result >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}

    ~generator_object()
    { deallocate_stack( stack_alloc_); }

    void exec_()
    {
        generator_self< Result > self( this);
        fn_( self);
    }

    void deallocate_object()
    { destroy( alloc_, this); }

private:
    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    generator_object( generator_object &);
    generator_object & operator=( generator_object const&);

    static void destroy( allocator_t const& alloc, generator_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

template< typename Fn, typename StackAllocator, typename Allocator, typename Result >
class generator_object< const reference_wrapper< Fn >, StackAllocator, Allocator, Result > :
    public generator_base< Result >
{
public:
    typedef typename Allocator::template rebind<
        generator_object<
            Fn, StackAllocator, Allocator, Result
        >
    >::other   allocator_t;

    generator_object( const reference_wrapper< Fn > fn, attributes const& attr,
                      StackAllocator const& stack_alloc, allocator_t const& alloc) :
        generator_base< Result >( attr, stack_alloc),
        fn_( fn),
        stack_alloc_( stack_alloc),
        alloc_( alloc)
    {}

    ~generator_object()
    { deallocate_stack( stack_alloc_); }

    void exec_()
    {
        generator_self< Result > self( this);
        fn_( self);
    }

    void deallocate_object()
    { destroy( alloc_, this); }

private:
    Fn              fn_;
    StackAllocator  stack_alloc_;
    allocator_t     alloc_;

    generator_object( generator_object &);
    generator_object & operator=( generator_object const&);

    static void destroy( allocator_t const& alloc, generator_object * p)
    {
        alloc.destroy( p);
        alloc.deallocate( p, 1);
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_GENERATOR_OBJECT_H
