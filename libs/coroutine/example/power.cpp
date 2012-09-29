
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/coroutine/all.hpp>

typedef boost::coro::coroutine< int() >         coro_t;
typedef boost::range_iterator< coro_t >::type   iterator_t;

int power( coro_t::self_t & self, int number, int exponent)
{
    int counter = 0;
    int result = 1;
    while ( counter++ < exponent - 1)
    {
            result = result * number;
            self( result);
    }
    return result * number;
}

int main()
{
    {
        std::cout << "using range functions" << std::endl;
        coro_t c( boost::bind( power, _1, 2, 8) );
        iterator_t e( boost::end( c) );
        for ( iterator_t i( boost::begin( c) ); i != e; ++i)
            std::cout << * i <<  " ";
    }

    {
        std::cout << "\nusing BOOST_FOREACH" << std::endl;
        coro_t c( boost::bind( power, _1, 2, 8) );
        BOOST_FOREACH( int i, c)
        { std::cout << i <<  " "; }
    }

    std::cout << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
