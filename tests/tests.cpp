#include <gtest/gtest.h>

int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
double divide(int a, int b) { 
    if (b == 0) throw std::invalid_argument("Division by zero");
    return static_cast<double>(a) / b; 
}

TEST(MathTest, Addition) {
    EXPECT_EQ(add(3, 4), 7);
    EXPECT_EQ(add(-3, 3), 0);
}

TEST(MathTest, Subtraction) {
    EXPECT_EQ(subtract(10, 4), 6);
    EXPECT_EQ(subtract(5, 8), -3);
}

TEST(MathTest, Multiplication) {
    EXPECT_EQ(multiply(3, 4), 12);
    EXPECT_EQ(multiply(-3, 3), -9);
}

TEST(MathTest, Division) {
    EXPECT_DOUBLE_EQ(divide(8, 2), 4.0);
    EXPECT_THROW(divide(5, 0), std::invalid_argument);
}

// Main function for GTest
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
