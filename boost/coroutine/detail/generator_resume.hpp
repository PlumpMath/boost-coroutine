
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_DETAIL_GENERATOR_RESUME_H
#define BOOST_CORO_DETAIL_GENERATOR_RESUME_H

#include <algorithm>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/optional.hpp>

#include <boost/coroutine/exceptions.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace coro {
namespace detail {

template< typename Result, typename D >
class generator_resume;

template< typename D >
class generator_resume< void, D >
{
public:
    generator_resume() :
        nxt_( false)
    {}

    void operator()()
    {
		D * dp = static_cast< D * >( this);
        BOOST_ASSERT( dp->impl_);

        fetch_();
    }

protected:
    void fetch_()
    {
		D * dp = static_cast< D * >( this);
        BOOST_ASSERT( dp->impl_);
        try
        {
            if ( ! dp->impl_->is_complete() )
            {
                if ( ! dp->impl_->is_started() ) dp->impl_->start();
                else dp->impl_->resume();
                nxt_ = true;
            }
            else nxt_ = false;
        }
        catch ( coroutine_terminated const&)
        { nxt_ = false; }
    }

    bool has_value_() const
    { return nxt_; }

    void swap_( generator_resume & other)
    { std::swap( nxt_, other.nxt_); }

private:
    bool nxt_;
};

template< typename Result, typename D >
class generator_resume
{
public:
    typedef Result result_t;

    result_t operator()()
    {
		D * dp = static_cast< D * >( this);
        BOOST_ASSERT( dp->impl_);

        result_t tmp( * result_);
        fetch_();
        return tmp;
    }

protected:
    void fetch_()
    {
		D * dp = static_cast< D * >( this);
        BOOST_ASSERT( dp->impl_);
        try
        {
            if ( ! dp->impl_->is_complete() )
            {
                if ( ! dp->impl_->is_started() ) result_ = dp->impl_->start();
                else result_ = dp->impl_->resume();
            }
            else result_ = none;
        }
        catch ( coroutine_terminated const&)
        { result_ = none; }
    }

    bool has_value_() const
    { return result_; }

    void swap_( generator_resume & other)
    { result_.swap( other.result_); }

private:
    optional< result_t >    result_;
};

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CORO_DETAIL_GENERATOR_RESUME_H
