// PoliciesExplicit.cpp
#include "PeakLogger.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include <iostream>
#include <string>

using namespace CinderPeak;

static void printHeader(const std::string &title) {
  std::cout << "---- " << title << " ----\n";
}

int main() {
  // Prepare two sample statuses (we will only use them in a few scenarios)
  PeakStatus alreadyExists(StatusCode::ALREADY_EXISTS, "entity already exists");
  PeakStatus vertexMissing(StatusCode::VERTEX_NOT_FOUND, "vertex id 42");

  // 1) Throw + LogConsole
  {
    printHeader("1) ErrorPolicy=Throw | LoggingPolicy=LogConsole");
    PolicyConfiguration cfg(PolicyConfiguration::Throw,
                            PolicyConfiguration::LogConsole, "");
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: starting scenario (Throw + Console)");
    // demonstrate exception handling here
    try {
      handler.handleException(alreadyExists); // expected: throw
      std::cout << "handleException returned (unexpected for Throw)\n";
    } catch (const std::exception &ex) {
      std::cout << "Caught (expected) exception: " << ex.what() << '\n';
    }
    Logger::shutdown();
    std::cout << '\n';
  }

  // 2) Throw + Silent
  {
    printHeader("2) ErrorPolicy=Throw | LoggingPolicy=Silent");
    PolicyConfiguration cfg(PolicyConfiguration::Throw,
                            PolicyConfiguration::Silent, "");
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: silent logs (no output expected)");
    // no exception demo here (to keep example short)
    Logger::shutdown();
    std::cout << '\n';
  }

  // 3) Throw + LogFile
  {
    printHeader("3) ErrorPolicy=Throw | LoggingPolicy=LogFile");
    std::string logfile = "policy_throw_file.log";
    PolicyConfiguration cfg(PolicyConfiguration::Throw,
                            PolicyConfiguration::LogFile, logfile);
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: this should go to file only");
    // demonstrate exception handling here
    try {
      handler.handleException(vertexMissing); // expected: throw
      std::cout << "handleException returned (unexpected for Throw)\n";
    } catch (const std::exception &ex) {
      std::cout << "Caught (expected) exception: " << ex.what() << '\n';
    }
    Logger::shutdown();
    std::cout << "File logs (if any) in: " << logfile << '\n';
    std::cout << '\n';
  }

  // 4) Throw + ConsoleAndFile
  {
    printHeader("4) ErrorPolicy=Throw | LoggingPolicy=ConsoleAndFile");
    std::string logfile = "policy_throw_both.log";
    PolicyConfiguration cfg(PolicyConfiguration::Throw,
                            PolicyConfiguration::ConsoleAndFile, logfile);
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: this should appear in console and file");
    // no exception demo here (keeps example compact)
    Logger::shutdown();
    std::cout << "File logs (if any) in: " << logfile << '\n';
    std::cout << '\n';
  }

  // 5) Ignore + LogConsole
  {
    printHeader("5) ErrorPolicy=Ignore | LoggingPolicy=LogConsole");
    PolicyConfiguration cfg(PolicyConfiguration::Ignore,
                            PolicyConfiguration::LogConsole, "");
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: console logging, errors ignored");
    // demonstrate that errors are ignored
    try {
      handler.handleException(alreadyExists); // expected: no throw
      std::cout << "handleException returned normally (expected for Ignore)\n";
    } catch (const std::exception &ex) {
      std::cout << "Caught unexpected exception: " << ex.what() << '\n';
    }
    Logger::shutdown();
    std::cout << '\n';
  }

  // 6) Ignore + Silent
  {
    printHeader("6) ErrorPolicy=Ignore | LoggingPolicy=Silent");
    PolicyConfiguration cfg(PolicyConfiguration::Ignore,
                            PolicyConfiguration::Silent, "");
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: silent logging, errors ignored (no output)");
    // no exception demo here
    Logger::shutdown();
    std::cout << '\n';
  }

  // 7) Ignore + LogFile
  {
    printHeader("7) ErrorPolicy=Ignore | LoggingPolicy=LogFile");
    std::string logfile = "policy_ignore_file.log";
    PolicyConfiguration cfg(PolicyConfiguration::Ignore,
                            PolicyConfiguration::LogFile, logfile);
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: file-only log, errors ignored");
    // no exception demo here
    Logger::shutdown();
    std::cout << "File logs (if any) in: " << logfile << '\n';
    std::cout << '\n';
  }

  // 8) Ignore + ConsoleAndFile
  {
    printHeader("8) ErrorPolicy=Ignore | LoggingPolicy=ConsoleAndFile");
    std::string logfile = "policy_ignore_both.log";
    PolicyConfiguration cfg(PolicyConfiguration::Ignore,
                            PolicyConfiguration::ConsoleAndFile, logfile);
    PolicyHandler handler(cfg);
    handler.log(INFO, "Info: console+file, errors ignored");
    // no exception demo here
    Logger::shutdown();
    std::cout << "File logs (if any) in: " << logfile << '\n';
    std::cout << '\n';
  }

  std::cout << "Done — explicit scenarios complete.\n";
  return 0;
}
