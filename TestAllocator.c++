// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/*
To test the program:
    % ls -al /usr/include/gtest/
    ...
    gtest.h
    ...

    % locate libgtest.a
    /usr/lib/libgtest.a

    % locate libpthread.a
    /usr/lib/x86_64-linux-gnu/libpthread.a
    /usr/lib32/libpthread.a

    % locate libgtest_main.a
    /usr/lib/libgtest_main.a

    % g++ -pedantic -std=c++0x -Wall TestAllocator.c++ -o TestAllocator -lgtest -lpthread -lgtest_main

    % valgrind TestAllocator > TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;};

template <typename A>
struct TestMyAllocator : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;};


typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
			std::allocator<char>,
            
            Allocator<int, 100>,
            Allocator<double, 100>,
			Allocator<char, 100>,
			
            Allocator<int, 99>,
			Allocator<double, 99>,
			Allocator<char, 99>,

			Allocator<int, 11>,
			Allocator<double,15>,
			Allocator<char, 9>
									 >
        my_types;

typedef testing::Types<
            Allocator<int, 100>,
            Allocator<double, 100>,
            Allocator<char, 100>,

            Allocator<int, 99>,
            Allocator<double, 99>,
            Allocator<char, 99>,

            Allocator<int, 11>,
            Allocator<double,15>,
            Allocator<char, 9>
                                     >
        other_types;

TYPED_TEST_CASE(TestAllocator, my_types);

TYPED_TEST_CASE(TestMyAllocator, other_types);

TYPED_TEST(TestAllocator, Ebola) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != 0) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator, MangeFever) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != 0) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

TYPED_TEST(TestAllocator, bubonic_plague) {	// fill the array
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;


    allocator_type x;
    const difference_type s = 92/sizeof(value_type);
    const value_type      v = 2;
	//std::cout << endl << "s is " << s;
    const pointer         b = x.allocate(s);
    if (b != 0) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

TYPED_TEST(TestAllocator, chimera) { // test multiple allocations 
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;
    allocator_type x;
    const difference_type s = 92/sizeof(value_type);
	//const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
	
	//string parameter = test_info->type_param(); 
	//cout << parameter 		
	for (int i = 1; i < s; ++i) {
		 x.deallocate(x.allocate(i), i);
    } 
 }
/*
TYPED_TEST(TestAllocator, medusa) { // tests allocation with null parameters
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;
    allocator_type x;
		x.deallocate(x.allocate(0), 5);
}
*/
TYPED_TEST(TestAllocator, minotaur) { // tests construction 

    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;
       try {
        allocator_type x;
      }
      catch (...) {
        ASSERT_FALSE(true);
      }
}

TYPED_TEST(TestAllocator, apocalypse) { // tests construction 

    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;
	allocator_type x;
	pointer p;
	std::vector<pointer> pointer_vector;
	int i = 215;
	while ( (i++ <= 215) && ((p = x.allocate(1))!= 0) ) {
		pointer_vector.push_back(p);
	}
	while (pointer_vector.size() != 0){
		x.deallocate(pointer_vector.back(),1);
		pointer_vector.pop_back();
	}
}
//-=-=-=-=-

TYPED_TEST(TestMyAllocator, One) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
	if (p != 0) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}
