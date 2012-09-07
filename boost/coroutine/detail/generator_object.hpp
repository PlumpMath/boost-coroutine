
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

template< typename Fn, typename StackAllocator, typename Result >
class generator_object : public generator_base< Result >
{
private:
    Fn              fn_;
    StackAllocator  alloc_;

    generator_object( generator_object &);
    generator_object & operator=( generator_object const&);

public:
#ifndef BOOST_NO_RVALUE_REFERENCES
    generator_object( Fn && fn, attributes const& attr, StackAllocator const& alloc) :
        generator_base< Result >( attr, alloc),
        fn_( static_cast< Fn && >( fn) ), alloc_( alloc)
    {}
#else
    generator_object( Fn fn, attributes const& attr, StackAllocator const& alloc) :
        generator_base< Result >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}

    generator_object( BOOST_RV_REF( Fn) fn, attributes const& attr, StackAllocator const& alloc) :
        generator_base< Result >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}
#endif

    ~generator_object()
    { deallocate( alloc_); }

    void exec_()
    {
        generator_self< Result > self( this);
        fn_( self);
    }
};

template< typename Fn, typename StackAllocator, typename Result >
class generator_object< reference_wrapper< Fn >, StackAllocator, Result > :
    public generator_base< Result >
{
private:
    Fn              fn_;
    StackAllocator  alloc_;

    generator_object( generator_object &);
    generator_object & operator=( generator_object const&);

public:
    generator_object( reference_wrapper< Fn > fn, attributes const& attr, StackAllocator const& alloc) :
        generator_base< Result >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}

    ~generator_object()
    { deallocate( alloc_); }

    void exec_()
    {
        generator_self< Result > self( this);
        fn_( self);
    }
};

template< typename Fn, typename StackAllocator, typename Result >
class generator_object< const reference_wrapper< Fn >, StackAllocator, Result > :
    public generator_base< Result >
{
private:
    Fn              fn_;
    StackAllocator  alloc_;

    generator_object( generator_object &);
    generator_object & operator=( generator_object const&);

public:
    generator_object( const reference_wrapper< Fn > fn, attributes const& attr, StackAllocator const& alloc) :
        generator_base< Result >( attr, alloc),
        fn_( fn), alloc_( alloc)
    {}

    ~generator_object()
    { deallocate( alloc_); }

    void exec_()
    {
        generator_self< Result > self( this);
        fn_( self);
    }
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_GENERATOR_OBJECT_H
