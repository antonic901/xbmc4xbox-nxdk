#include <exception>
#include <iostream>

extern "C" void __std_terminate() { std::terminate(); }
extern "C" void __CxxFrameHandler3() {}
extern "C" void _CxxThrowException(void*, void*) {}

void testException()
{
  try
  {
    throw std::runtime_error("Test exception!");
  }
  catch (const std::exception& e)
  {
    // Catch standard exceptions
  }
  catch (...)
  {
    // Catch any other unknown exceptions
  }
}
