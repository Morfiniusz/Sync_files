//
// Created by bartl on 22.03.2023.
//
#include <gtest/gtest.h>
#include "./../ThreadPool.h" // Include your ThreadPool header file

// Add your test cases here, for example:
TEST(ThreadPoolTest, Constructor) {
ThreadPool pool1(2);
// Replace getThreadCount() with an appropriate getter method in your ThreadPool class
ASSERT_EQ(pool1.getThreadCount(), 2);

ThreadPool pool2(4);
ASSERT_EQ(pool2.getThreadCount(), 4);
}

// Add more tests as needed

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}