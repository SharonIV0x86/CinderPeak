#include "CinderExceptions.hpp"
#include "DummyGraphBuilder.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/Utils.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;
using namespace CinderPeak::PeakExceptions;

class ExceptionPropagationTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

// ===== handle_exception_map with shouldThrow=false (default) =====

TEST_F(ExceptionPropagationTest, NoThrowWhenDisabled) {
  PeakStatus status = PeakStatus::VertexNotFound("test missing vertex");
  EXPECT_NO_THROW(Exceptions::handle_exception_map(status));
}

TEST_F(ExceptionPropagationTest, NoThrowForAnyStatusCodeWhenDisabled) {
  EXPECT_NO_THROW(Exceptions::handle_exception_map(
      PeakStatus::VertexNotFound("v missing")));
  EXPECT_NO_THROW(
      Exceptions::handle_exception_map(PeakStatus::EdgeNotFound("e missing")));
  EXPECT_NO_THROW(Exceptions::handle_exception_map(
      PeakStatus::VertexAlreadyExists("dup v")));
  EXPECT_NO_THROW(
      Exceptions::handle_exception_map(PeakStatus::EdgeAlreadyExists("dup e")));
  EXPECT_NO_THROW(
      Exceptions::handle_exception_map(PeakStatus::InvalidArgument("bad")));
  EXPECT_NO_THROW(
      Exceptions::handle_exception_map(PeakStatus::InternalError("err")));
  EXPECT_NO_THROW(
      Exceptions::handle_exception_map(PeakStatus::AlreadyExists("exists")));
  EXPECT_NO_THROW(
      Exceptions::handle_exception_map(PeakStatus::Unimplemented("todo")));
}

// ===== handle_exception_map with shouldThrow=true =====

TEST_F(ExceptionPropagationTest, ThrowsVertexNotFoundException) {
  PeakStatus status = PeakStatus::VertexNotFound("vertex X not found");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               VertexNotFoundException);
}

TEST_F(ExceptionPropagationTest, ThrowsEdgeNotFoundException) {
  PeakStatus status = PeakStatus::EdgeNotFound("edge (1,2) not found");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               EdgeNotFoundException);
}

TEST_F(ExceptionPropagationTest, ThrowsVertexAlreadyExistsException) {
  PeakStatus status =
      PeakStatus::VertexAlreadyExists("vertex 1 already exists");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               VertexAlreadyExistsException);
}

TEST_F(ExceptionPropagationTest, ThrowsEdgeAlreadyExistsException) {
  PeakStatus status = PeakStatus::EdgeAlreadyExists("edge (1,2) exists");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               EdgeAlreadyExistsException);
}

TEST_F(ExceptionPropagationTest, ThrowsInvalidArgumentException) {
  PeakStatus status = PeakStatus::InvalidArgument("self loops not allowed");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               InvalidArgumentException);
}

TEST_F(ExceptionPropagationTest, ThrowsInternalErrorException) {
  PeakStatus status = PeakStatus::InternalError("unexpected failure");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               InternalErrorException);
}

TEST_F(ExceptionPropagationTest, ThrowsAlreadyExistsException) {
  PeakStatus status = PeakStatus::AlreadyExists("resource duplicate");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               AlreadyExistsException);
}

TEST_F(ExceptionPropagationTest, ThrowsUnimplementedException) {
  PeakStatus status = PeakStatus::Unimplemented("method not available");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               UnimplementedException);
}

TEST_F(ExceptionPropagationTest, ThrowsNotFoundException) {
  PeakStatus status = PeakStatus::NotFound("resource not found");
  EXPECT_THROW(Exceptions::handle_exception_map(status, true),
               NotFoundException);
}

// ===== Contextual message propagation =====

TEST_F(ExceptionPropagationTest, PreservesContextualMessage) {
  std::string expectedMsg = "Vertex 'X' already exists in graph";
  PeakStatus status = PeakStatus::VertexAlreadyExists(expectedMsg);
  try {
    Exceptions::handle_exception_map(status, true);
    FAIL() << "Expected VertexAlreadyExistsException";
  } catch (const VertexAlreadyExistsException &e) {
    std::string what = e.what();
    EXPECT_TRUE(what.find(expectedMsg) != std::string::npos);
  }
}

TEST_F(ExceptionPropagationTest, PreservesVertexNotFoundContext) {
  std::string expectedMsg = "Source vertex does not exist: Vertex(42)";
  PeakStatus status = PeakStatus::VertexNotFound(expectedMsg);
  try {
    Exceptions::handle_exception_map(status, true);
    FAIL() << "Expected VertexNotFoundException";
  } catch (const VertexNotFoundException &e) {
    std::string what = e.what();
    EXPECT_TRUE(what.find(expectedMsg) != std::string::npos);
  }
}

TEST_F(ExceptionPropagationTest, PreservesEdgeNotFoundContext) {
  std::string expectedMsg = "Edge does not exist: Edge(1 -> 2)";
  PeakStatus status = PeakStatus::EdgeNotFound(expectedMsg);
  try {
    Exceptions::handle_exception_map(status, true);
    FAIL() << "Expected EdgeNotFoundException";
  } catch (const EdgeNotFoundException &e) {
    std::string what = e.what();
    EXPECT_TRUE(what.find(expectedMsg) != std::string::npos);
  }
}

// ===== ErrorCode on exceptions =====

TEST_F(ExceptionPropagationTest, VertexNotFoundExceptionHasErrorCode) {
  try {
    Exceptions::handle_exception_map(PeakStatus::VertexNotFound("missing"),
                                     true);
    FAIL() << "Expected VertexNotFoundException";
  } catch (const GraphException &e) {
    EXPECT_EQ(e.errorCode(), ErrorCode::VERTEX_NOT_FOUND);
  }
}

TEST_F(ExceptionPropagationTest, EdgeNotFoundExceptionHasErrorCode) {
  try {
    Exceptions::handle_exception_map(PeakStatus::EdgeNotFound("missing"), true);
    FAIL() << "Expected EdgeNotFoundException";
  } catch (const GraphException &e) {
    EXPECT_EQ(e.errorCode(), ErrorCode::EDGE_NOT_FOUND);
  }
}

TEST_F(ExceptionPropagationTest, DuplicateVertexExceptionHasErrorCode) {
  try {
    Exceptions::handle_exception_map(PeakStatus::VertexAlreadyExists("dup"),
                                     true);
    FAIL() << "Expected VertexAlreadyExistsException";
  } catch (const GraphException &e) {
    EXPECT_EQ(e.errorCode(), ErrorCode::DUPLICATE_VERTEX);
  }
}

TEST_F(ExceptionPropagationTest, EdgeAlreadyExistsExceptionHasErrorCode) {
  try {
    Exceptions::handle_exception_map(PeakStatus::EdgeAlreadyExists("dup"),
                                     true);
    FAIL() << "Expected EdgeAlreadyExistsException";
  } catch (const GraphException &e) {
    EXPECT_EQ(e.errorCode(), ErrorCode::INVALID_EDGE);
  }
}

TEST_F(ExceptionPropagationTest, UnimplementedExceptionHasErrorCode) {
  try {
    Exceptions::handle_exception_map(PeakStatus::Unimplemented("todo"), true);
    FAIL() << "Expected UnimplementedException";
  } catch (const GraphException &e) {
    EXPECT_EQ(e.errorCode(), ErrorCode::NOT_IMPLEMENTED);
  }
}

// ===== CinderGraph-level exception propagation =====

TEST_F(ExceptionPropagationTest, GraphAddVertexDuplicateThrowsWhenEnabled) {
  auto graph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);
  graph.setThrowExceptions(true);
  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_THROW(graph.addVertex(1), VertexAlreadyExistsException);
}

TEST_F(ExceptionPropagationTest, GraphRemoveVertexNotFoundThrowsWhenEnabled) {
  auto graph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);
  graph.setThrowExceptions(true);
  graph.addVertex(1);
  EXPECT_THROW(graph.removeVertex(999), VertexNotFoundException);
}

TEST_F(ExceptionPropagationTest, GraphDoesNotThrowWhenDisabled) {
  auto graph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);
  graph.setThrowExceptions(false);
  graph.addVertex(1);
  EXPECT_NO_THROW(graph.addVertex(1));
  EXPECT_FALSE(graph.addVertex(1).second);
}

TEST_F(ExceptionPropagationTest, GraphAddEdgeMissingVerticesThrows) {
  auto graph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);
  graph.setThrowExceptions(true);
  graph.addVertex(1);
  EXPECT_THROW(graph.addEdge(1, 999, 5), VertexNotFoundException);
}

TEST_F(ExceptionPropagationTest, GraphRemoveEdgeMissingVertexThrows) {
  auto graph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);
  graph.setThrowExceptions(true);
  graph.addVertex(1);
  graph.addVertex(2);
  EXPECT_THROW(graph.removeEdge(1, 999), VertexNotFoundException);
}

TEST_F(ExceptionPropagationTest, GraphGetEdgeMissingVertexThrows) {
  auto graph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);
  graph.setThrowExceptions(true);
  graph.addVertex(1);
  graph.addVertex(2);
  EXPECT_THROW(graph.getEdge(1, 999), VertexNotFoundException);
}
