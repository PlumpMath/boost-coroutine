
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CORO_EXCEPTIONS_H
#define BOOST_CORO_EXCEPTIONS_H

#include <stdexcept>

#include <boost/exception/exception.hpp>

namespace boost {
namespace coro {

class coroutine_terminated :
    public virtual exception,
    public virtual std::exception
{};

}}

#endif // BOOST_CORO_EXCEPTIONS_H
