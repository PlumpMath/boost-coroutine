
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/coroutine/all.hpp>

#include "tree.h"

node::ptr_t create_tree()
{
    return branch::create(
        leaf::create( "A"),
        branch::create(
            leaf::create( "B"),
            leaf::create( "C") ) );
}

int main( int argc, char * argv[])
{
    {
        node::ptr_t root = create_tree();
        gen_t gen(
            boost::bind(
                enumerate_leafs, _1, root) );
        while ( boost::optional< leaf & > val = gen() ) {
            std::cout << val->value <<  " ";
        }
    }

    std::cout << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
