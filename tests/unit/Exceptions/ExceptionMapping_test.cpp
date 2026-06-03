#include "CinderExceptions.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include "StorageEngine/Utils.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;
using namespace CinderPeak::PeakExceptions;

// ---------------------------------------------------------------------------
// GraphException structure
// ---------------------------------------------------------------------------

TEST(ExceptionTest, GraphException_IsRuntimeError) {
  GraphException ex("test");
  EXPECT_TRUE(dynamic_cast<std::runtime_error *>(&ex));
}

TEST(ExceptionTest, GraphException_CarriesErrorCode) {
  GraphException ex("resource not found", ErrorCode::NotFound);
  EXPECT_EQ(ex.errorCode(), ErrorCode::NotFound);
  EXPECT_EQ(std::string(ex.what()), "resource not found");
}

TEST(ExceptionTest, GraphException_DefaultErrorCodeIsNone) {
  GraphException ex("msg");
  EXPECT_EQ(ex.errorCode(), ErrorCode::None);
}

TEST(ExceptionTest, NotFoundException_Format) {
  NotFoundException ex("vertex missing");
  EXPECT_EQ(std::string(ex.what()), "Resource not found: vertex missing");
  EXPECT_EQ(ex.errorCode(), ErrorCode::NotFound);
}

TEST(ExceptionTest, InvalidArgumentException_Format) {
  InvalidArgumentException ex("negative weight");
  EXPECT_EQ(std::string(ex.what()), "Invalid argument: negative weight");
  EXPECT_EQ(ex.errorCode(), ErrorCode::InvalidArgument);
}

TEST(ExceptionTest, VertexAlreadyExistsException_Format) {
  VertexAlreadyExistsException ex("id=42");
  EXPECT_EQ(std::string(ex.what()), "Vertex already exists: id=42");
  EXPECT_EQ(ex.errorCode(), ErrorCode::VertexAlreadyExists);
}

TEST(ExceptionTest, EdgeAlreadyExistsException_Format) {
  EdgeAlreadyExistsException ex("(1,2)");
  EXPECT_EQ(std::string(ex.what()), "Edge already exists: (1,2)");
  EXPECT_EQ(ex.errorCode(), ErrorCode::EdgeAlreadyExists);
}

TEST(ExceptionTest, EdgeNotFoundException_Format) {
  EdgeNotFoundException ex("(5,10)");
  EXPECT_EQ(std::string(ex.what()), "Edge not found: (5,10)");
  EXPECT_EQ(ex.errorCode(), ErrorCode::EdgeNotFound);
}

TEST(ExceptionTest, VertexNotFoundException_Format) {
  VertexNotFoundException ex("id=99");
  EXPECT_EQ(std::string(ex.what()), "Vertex not found: id=99");
  EXPECT_EQ(ex.errorCode(), ErrorCode::VertexNotFound);
}

TEST(ExceptionTest, InternalErrorException_Format) {
  InternalErrorException ex("corrupt data");
  EXPECT_EQ(std::string(ex.what()), "Internal error: corrupt data");
  EXPECT_EQ(ex.errorCode(), ErrorCode::InternalError);
}

TEST(ExceptionTest, UnimplementedException_Format) {
  UnimplementedException ex("batch insert");
  EXPECT_EQ(std::string(ex.what()), "Unimplemented feature: batch insert");
  EXPECT_EQ(ex.errorCode(), ErrorCode::Unimplemented);
}

TEST(ExceptionTest, AlreadyExistsException_Format) {
  AlreadyExistsException ex("graph already exists");
  EXPECT_EQ(std::string(ex.what()),
            "Resource already exists: graph already exists");
  EXPECT_EQ(ex.errorCode(), ErrorCode::AlreadyExists);
}

TEST(ExceptionTest, UnknownException_Format) {
  UnknownException ex;
  EXPECT_EQ(ex.errorCode(), ErrorCode::InternalError);
}

// ---------------------------------------------------------------------------
// handle_exception_map — status → exception mapping
// ---------------------------------------------------------------------------

TEST(ExceptionTest, HandleExceptionMap_OK_DoesNotThrow) {
  EXPECT_NO_THROW(Exceptions::handle_exception_map(PeakStatus::OK()));
}

TEST(ExceptionTest, HandleExceptionMap_NotFound_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(PeakStatus::NotFound("vertex"), "",
                                         true);
      },
      NotFoundException);
}

TEST(ExceptionTest, HandleExceptionMap_Unimplemented_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(PeakStatus::Unimplemented("batch"), "",
                                         true);
      },
      UnimplementedException);
}

TEST(ExceptionTest, HandleExceptionMap_AlreadyExists_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(PeakStatus::AlreadyExists("graph"), "",
                                         true);
      },
      AlreadyExistsException);
}

TEST(ExceptionTest, HandleExceptionMap_VertexAlreadyExists_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(
            PeakStatus::VertexAlreadyExists("id=1"), "", true);
      },
      VertexAlreadyExistsException);
}

TEST(ExceptionTest, HandleExceptionMap_VertexNotFound_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(PeakStatus::VertexNotFound("id=42"),
                                         "", true);
      },
      VertexNotFoundException);
}

TEST(ExceptionTest, HandleExceptionMap_EdgeAlreadyExists_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(PeakStatus::EdgeAlreadyExists("(1,2)"),
                                         "", true);
      },
      EdgeAlreadyExistsException);
}

TEST(ExceptionTest, HandleExceptionMap_EdgeNotFound_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(PeakStatus::EdgeNotFound("(3,4)"), "",
                                         true);
      },
      EdgeNotFoundException);
}

TEST(ExceptionTest, HandleExceptionMap_InvalidArgument_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(
            PeakStatus::InvalidArgument("bad input"), "", true);
      },
      InvalidArgumentException);
}

TEST(ExceptionTest, HandleExceptionMap_InternalError_Throws) {
  EXPECT_THROW(
      {
        Exceptions::handle_exception_map(PeakStatus::InternalError("oops"), "",
                                         true);
      },
      InternalErrorException);
}

TEST(ExceptionTest, HandleExceptionMap_CarriesContext) {
  try {
    Exceptions::handle_exception_map(PeakStatus::VertexNotFound("id=7"),
                                     "removeVertex", true);
  } catch (const VertexNotFoundException &ex) {
    std::string msg = ex.what();
    EXPECT_TRUE(msg.find("removeVertex") != std::string::npos);
    EXPECT_TRUE(msg.find("id=7") != std::string::npos);
    EXPECT_EQ(ex.errorCode(), ErrorCode::VertexNotFound);
  }
}

TEST(ExceptionTest, HandleExceptionMap_ContextWithoutMessage) {
  try {
    Exceptions::handle_exception_map(PeakStatus::VertexAlreadyExists(),
                                     "addVertex", true);
  } catch (const VertexAlreadyExistsException &ex) {
    std::string msg = ex.what();
    EXPECT_TRUE(msg.find("addVertex") != std::string::npos);
    EXPECT_EQ(ex.errorCode(), ErrorCode::VertexAlreadyExists);
  }
}

TEST(ExceptionTest, HandleExceptionMap_EmptyStatusMessage) {
  try {
    Exceptions::handle_exception_map(PeakStatus(StatusCode::INTERNAL_ERROR, ""),
                                     "process", true);
  } catch (const InternalErrorException &ex) {
    std::string msg = ex.what();
    EXPECT_TRUE(msg.find("process") != std::string::npos);
  }
}

// ---------------------------------------------------------------------------
// ErrorCode helpers
// ---------------------------------------------------------------------------

TEST(ErrorCodeTest, ToErrorCode_Conversion) {
  EXPECT_EQ(toErrorCode(StatusCode::OK), ErrorCode::None);
  EXPECT_EQ(toErrorCode(StatusCode::NOT_FOUND), ErrorCode::NotFound);
  EXPECT_EQ(toErrorCode(StatusCode::INVALID_ARGUMENT),
            ErrorCode::InvalidArgument);
  EXPECT_EQ(toErrorCode(StatusCode::VERTEX_ALREADY_EXISTS),
            ErrorCode::VertexAlreadyExists);
  EXPECT_EQ(toErrorCode(StatusCode::INTERNAL_ERROR), ErrorCode::InternalError);
  EXPECT_EQ(toErrorCode(StatusCode::EDGE_NOT_FOUND), ErrorCode::EdgeNotFound);
  EXPECT_EQ(toErrorCode(StatusCode::VERTEX_NOT_FOUND),
            ErrorCode::VertexNotFound);
  EXPECT_EQ(toErrorCode(StatusCode::UNIMPLEMENTED), ErrorCode::Unimplemented);
  EXPECT_EQ(toErrorCode(StatusCode::ALREADY_EXISTS), ErrorCode::AlreadyExists);
  EXPECT_EQ(toErrorCode(StatusCode::EDGE_ALREADY_EXISTS),
            ErrorCode::EdgeAlreadyExists);
}

TEST(ErrorCodeTest, ErrorCodeToString) {
  EXPECT_EQ(errorCodeToString(ErrorCode::None), "OK");
  EXPECT_EQ(errorCodeToString(ErrorCode::NotFound), "Resource not found");
  EXPECT_EQ(errorCodeToString(ErrorCode::InvalidArgument), "Invalid argument");
  EXPECT_EQ(errorCodeToString(ErrorCode::VertexAlreadyExists),
            "Vertex already exists");
}

TEST(ErrorCodeTest, ErrorCodeToString_UnknownCode) {
  auto unknown = static_cast<ErrorCode>(999);
  EXPECT_EQ(errorCodeToString(unknown), "Unknown error");
}

TEST(ErrorCodeTest, PeakStatus_ToErrorCode) {
  PeakStatus status(StatusCode::VERTEX_NOT_FOUND, "custom msg");
  EXPECT_EQ(toErrorCode(status.code()), ErrorCode::VertexNotFound);
}
