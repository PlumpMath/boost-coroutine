
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/move/move.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>

#include <boost/coroutine/all.hpp>

namespace coro = boost::coro;
namespace ctx = boost::context;

int value1 = 0;
std::string value2 = "";

typedef coro::coroutine< void() > coro_void_void;
typedef coro::coroutine< int() > coro_int_void;
typedef coro::coroutine< std::string() > coro_string_void;
typedef coro::coroutine< void(int) > coro_void_int;
typedef coro::coroutine< void(std::string const&) > coro_void_string;
typedef coro::coroutine< double(double,double) > coro_double;
typedef coro::coroutine< int(int,int) > coro_int;
typedef coro::coroutine< int*(int*) > coro_ptr;
typedef coro::coroutine< int&(int&) > coro_ref;
typedef coro::coroutine< boost::tuple<int&,int&>(int&,int&) > coro_tuple;

struct X : private boost::noncopyable
{
    X() { value1 = 7; }
    ~X() { value1 = 0; }
};

class copyable
{
private:
    int i_;

public:
    copyable( int i) :
        i_( i)
    {}

    int operator()( coro_int_void::self_t &)
    { return i_; }
};

class moveable
{
private:
    bool    state_;
    int     i_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE( moveable);

public:
    moveable() :
        state_( false), i_( 0)
    {}

    moveable( int i) :
        state_( false), i_( i)
    {}

    moveable( BOOST_RV_REF( moveable) other) :
        state_( false), i_( 0)
    {
        std::swap( state_, other.state_);
        std::swap( i_, other.i_);
    }

    moveable & operator=( BOOST_RV_REF( moveable) other)
    {
        if ( this == & other) return * this;
        moveable tmp( boost::move( other) );
        std::swap( state_, tmp.state_);
        std::swap( i_, tmp.i_);
        return * this;
    }

    int operator()( coro_int_void::self_t &)
    { return i_; }
};

struct my_exception {};

void f1( coro_void_void::self_t & s)
{}

void f2( coro_void_void::self_t &)
{ ++value1; }

void f3( coro_void_void::self_t & self)
{
    ++value1;
    self();
    ++value1;
}

void f4( coro_int_void::self_t & self)
{
    self( 3);
    self( 7);
}

void f5( coro_string_void::self_t & self)
{
    std::string res("abc");
    self( res);
    res = "xyz";
    self( res);
}

void f6( coro_void_int::self_t & self, int i)
{
    value1 = i;
}

void f7( coro_void_string::self_t & self, std::string const& str)
{
    value2 = str;
}

void f8( coro_double::self_t & self, double a, double b)
{
    double tmp = a + b;
    boost::tuple< double, double > ret = self( tmp).get();
    self( ret.get< 0 >() + ret.get< 1 >() );
}

void f9( coro_ptr::self_t & self, int * a)
{
    self( a);
}

void f10( coro_ref::self_t & self, int & a)
{
    self( a);
}

void f11( coro_tuple::self_t & self, int & a, int & b)
{
    boost::tuple<int&,int&> tpl( a, b);
    self( tpl);
}

void f12( coro_int::self_t & self, int a, int b)
{
    X x;
    int tmp = a + b;
    boost::tuple< int, int > ret = self( tmp).get();
    value1 = 1;
    self( ret.get< 0 >() + ret.get< 1 >() );
}

template< typename E >
void f14( coro_int_void::self_t & self, E const& e)
{
    throw e;
}

void f15( coro_int_void::self_t & self)
{}

void test_move()
{
    {
        coro_void_void coro1;
        coro_void_void coro2( boost::bind( f1, _1) );
        BOOST_CHECK( ! coro1);
        BOOST_CHECK( coro1.empty() );
        BOOST_CHECK( coro2);
        BOOST_CHECK( ! coro2.empty() );
        coro1 = boost::move( coro2);
        BOOST_CHECK( coro1);
        BOOST_CHECK( ! coro1.empty() );
        BOOST_CHECK( ! coro2);
        BOOST_CHECK( coro2.empty() );
    }

    {
        copyable cp( 3);
        coro_int_void coro( cp);
    }

    {
        moveable mv( 7);
        coro_int_void coro( boost::move( mv) );
    }
}

void test_complete()
{
    value1 = 0;

    coro_void_void coro( boost::bind( f2, _1) );
    BOOST_CHECK( coro);
    coro();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( ( int)1, value1);
}

void test_jump()
{
    value1 = 0;

    coro_void_void coro( boost::bind( f3, _1) );
    BOOST_CHECK( coro);
    coro();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( ( int)1, value1);
    coro();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( ( int)2, value1);
}

void test_result_int()
{
    coro_int_void coro( boost::bind( f4, _1) );
    BOOST_CHECK( coro);
    int result = coro().get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( 3, result);
    result = coro().get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( 7, result);
}

void test_result_string()
{
    coro_string_void coro( boost::bind( f5, _1) );
    BOOST_CHECK( coro);
    std::string result = coro().get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( std::string("abc"), result);
    result = coro().get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( std::string("xyz"), result);
}

void test_arg_int()
{
    value1 = 0;

    coro_void_int coro( boost::bind( f6, _1, _2) );
    BOOST_CHECK( coro);
    coro( 3);
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( 3, value1);
}

void test_arg_string()
{
    value2 = "";

    coro_void_string coro( boost::bind( f7, _1, _2) );
    BOOST_CHECK( coro);
    coro( std::string("abc") );
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( std::string("abc"), value2);
}

void test_fp()
{
    coro_double coro( boost::bind( f8, _1, _2, _3) );
    BOOST_CHECK( coro);
    double res = coro( 7.35, 3.14).get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( ( double) 10.49, res);
    res = coro( 1.15, 3.14).get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( ( double) 4.29, res);
}

void test_ptr()
{
    coro_ptr coro( boost::bind( f9, _1, _2) );
    BOOST_CHECK( coro);
    int a = 3;
    int * res = coro( & a).get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( & a, res);
    res = 0;
    res = coro( & a).get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( & a, res);
}

void test_ref()
{
    coro_ref coro( boost::bind( f10, _1, _2) );
    BOOST_CHECK( coro);
    int a = 3;
    int & res = coro( a).get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( & a, & res);
    res = 0;
    res = coro( a).get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( & a, & res);
}

void test_tuple()
{
    coro_tuple coro( boost::bind( f11, _1, _2, _3) );
    BOOST_CHECK( coro);
    int a = 3, b = 7;
    boost::tuple<int&,int&> res = coro( a, b).get();
    BOOST_CHECK( coro);
    BOOST_CHECK_EQUAL( & a, & res.get< 0 >() );
    BOOST_CHECK_EQUAL( & b, & res.get< 1 >() );
    res = coro( a, b).get();
    BOOST_CHECK( ! coro);
    BOOST_CHECK_EQUAL( & a, & res.get< 0 >() );
    BOOST_CHECK_EQUAL( & b, & res.get< 1 >() );
}

void test_unwind()
{
    value1 = 0;
    {
        coro_int coro( boost::bind( f12, _1, _2, _3) );
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 0, value1);
        int res = coro( 3, 7).get();
        BOOST_CHECK_EQUAL( ( int) 7, value1);
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 10, res);
    }
    BOOST_CHECK_EQUAL( ( int) 0, value1);
}

void test_no_unwind()
{
    value1 = 0;
    {
        coro_int coro(
            boost::bind( f12, _1, _2, _3),
            coro::attributes(
                ctx::guarded_stack_allocator::default_stacksize(),
                coro::no_stack_unwind) );
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 0, value1);
        int res = coro( 3, 7).get();
        BOOST_CHECK( coro);
        BOOST_CHECK_EQUAL( ( int) 10, res);
    }
    BOOST_CHECK_EQUAL( ( int) 7, value1);
}

void test_exceptions()
{
    bool thrown = false;
    std::runtime_error ex("abc");
    coro_int_void coro( boost::bind( f14< std::runtime_error >, _1, ex) );
    try
    {
        BOOST_CHECK( coro);
        coro();
        BOOST_CHECK( false);
    }
    catch ( std::runtime_error const&)
    { thrown = true; }
    catch ( std::exception const&)
    {}
    catch (...)
    {}
    BOOST_CHECK( thrown);
    BOOST_CHECK( ! coro);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
    boost::unit_test::test_suite * test =
        BOOST_TEST_SUITE("Boost.coroutine: coroutine test suite");

   test->add( BOOST_TEST_CASE( & test_move) );
   test->add( BOOST_TEST_CASE( & test_complete) );
   test->add( BOOST_TEST_CASE( & test_jump) );
   test->add( BOOST_TEST_CASE( & test_result_int) );
   test->add( BOOST_TEST_CASE( & test_result_string) );
   test->add( BOOST_TEST_CASE( & test_arg_int) );
   test->add( BOOST_TEST_CASE( & test_arg_string) );
   test->add( BOOST_TEST_CASE( & test_fp) );
   test->add( BOOST_TEST_CASE( & test_ptr) );
   test->add( BOOST_TEST_CASE( & test_ref) );
   test->add( BOOST_TEST_CASE( & test_tuple) );
   test->add( BOOST_TEST_CASE( & test_unwind) );
   test->add( BOOST_TEST_CASE( & test_no_unwind) );
   test->add( BOOST_TEST_CASE( & test_exceptions) );

    return test;
}
