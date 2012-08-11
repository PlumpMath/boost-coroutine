
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#include <boost/bind.hpp>

#include "tree.h"

bool match_trees( gen_t & te1, gen_t & te2)
{
    if ( ! te1 && ! te2) return true;
    if ( ! te1 || ! te2) return false;
    if ( te1().value == te2().value) return match_trees( te1, te2);
    else return false;
}

std::pair< node::ptr_t, node::ptr_t > create_eq_trees()
{
    branch::ptr_t tree1 = branch::create(
        leaf::create( "A"),
        branch::create(
            leaf::create( "B"),
            leaf::create( "C") ) );

    branch::ptr_t tree2 = branch::create(
        branch::create(
            leaf::create( "A"),
            leaf::create( "B") ),
        leaf::create( "C") );

    return std::make_pair( tree1, tree2);
}

std::pair< node::ptr_t, node::ptr_t > create_diff_trees()
{
    branch::ptr_t tree1 = branch::create(
        leaf::create( "A"),
        branch::create(
            leaf::create( "B"),
            leaf::create( "C") ) );

    branch::ptr_t tree2 = branch::create(
        branch::create(
            leaf::create( "A"),
            leaf::create( "X") ),
        leaf::create( "C") );

    return std::make_pair( tree1, tree2);
}

int main()
{
    {
        std::pair< node::ptr_t, node::ptr_t > pt = create_eq_trees();
        gen_t te1( boost::bind( enumerate_leafs, _1, pt.first) );
        gen_t te2( boost::bind( enumerate_leafs, _1, pt.second) );
        bool result = match_trees( te1, te2);
        std::cout << std::boolalpha << "eq. trees matched == " << result << std::endl;
    }
    {
        std::pair< node::ptr_t, node::ptr_t > pt = create_diff_trees();
        gen_t te1( boost::bind( enumerate_leafs, _1, pt.first) );
        gen_t te2( boost::bind( enumerate_leafs, _1, pt.second) );
        bool result = match_trees( te1, te2);
        std::cout << std::boolalpha << "diff. trees matched == " << result << std::endl;
    }

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}
