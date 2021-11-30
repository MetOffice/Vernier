/*----------------------------------------------------------------------------*\
 (C) Crown copyright 2021 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
--------------------------------------------------------------------------------
 Description:
 A basic unit test example using GoogleTest.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}
