// Unit tests for CinderPeak::dbg() — StorageEngine/DbgUtils.hpp

#include "StorageEngine/DbgUtils.hpp"
#include <gtest/gtest.h>
#include <optional>
#include <string>

using namespace CinderPeak;

// Primitives

TEST(DbgTest, Int) {
  EXPECT_EQ(dbg(42), "42");
  EXPECT_EQ(dbg(-7), "-7");
  EXPECT_EQ(dbg(0), "0");
}

TEST(DbgTest, Float) {
  std::string result = dbg(3.14f);
  EXPECT_EQ(result.substr(0, 4), "3.14");
}

TEST(DbgTest, Double) {
  std::string result = dbg(2.718);
  EXPECT_EQ(result.substr(0, 5), "2.718");
}

TEST(DbgTest, Bool_True) { EXPECT_EQ(dbg(true), "true"); }

TEST(DbgTest, Bool_False) { EXPECT_EQ(dbg(false), "false"); }

TEST(DbgTest, StdString) {
  EXPECT_EQ(dbg(std::string("hello")), "hello");
  EXPECT_EQ(dbg(std::string("")), "");
}

// Enum

enum class Color { Red = 0, Green = 1, Blue = 2 };

TEST(DbgTest, Enum) {
  EXPECT_EQ(dbg(Color::Red), "0");
  EXPECT_EQ(dbg(Color::Green), "1");
  EXPECT_EQ(dbg(Color::Blue), "2");
}

// Unweighted

TEST(DbgTest, Unweighted) { EXPECT_EQ(dbg(Unweighted{}), "Unweighted"); }

// std::optional<T>

TEST(DbgTest, Optional_HasValue) {
  std::optional<int> opt = 7;
  EXPECT_EQ(dbg(opt), "7");
}

TEST(DbgTest, Optional_Empty) {
  std::optional<int> opt;
  EXPECT_EQ(dbg(opt), "nullopt");
}

TEST(DbgTest, Optional_String_HasValue) {
  std::optional<std::string> opt = std::string("world");
  EXPECT_EQ(dbg(opt), "world");
}

// Pointer types

TEST(DbgTest, Pointer_NonNull) {
  int x = 99;
  int *p = &x;
  EXPECT_EQ(dbg(p), "99");
}

TEST(DbgTest, Pointer_Null) {
  int *p = nullptr;
  EXPECT_EQ(dbg(p), "null");
}

// CinderVertex / CinderEdge (has __id_)

TEST(DbgTest, CinderVertex) {
  CinderVertex v("TestNode");
  std::string result = dbg(v);
  ASSERT_GT(result.size(), 3u);
  EXPECT_EQ(result.substr(0, 3), "id:");
  EXPECT_NO_THROW(std::stoul(result.substr(3)));
}

TEST(DbgTest, CinderEdge) {
  CinderEdge e("TestEdge");
  std::string result = dbg(e);
  ASSERT_GT(result.size(), 3u);
  EXPECT_EQ(result.substr(0, 3), "id:");
  EXPECT_NO_THROW(std::stoul(result.substr(3)));
}

// Opaque user-defined type (fallback)

struct OpaqueStruct {
  int data{42};
};

TEST(DbgTest, Fallback_OpaqueStruct) {
  OpaqueStruct obj;
  std::string result = dbg(obj);
  ASSERT_GT(result.size(), 4u);
  EXPECT_EQ(result.substr(0, 4), "obj@");
  EXPECT_NO_THROW(std::stoull(result.substr(4)));
  EXPECT_NE(std::stoull(result.substr(4)), 0u);
}

TEST(DbgTest, Fallback_TwoDistinctObjects_DifferentAddresses) {
  OpaqueStruct a, b;
  EXPECT_NE(dbg(a), dbg(b));
}
