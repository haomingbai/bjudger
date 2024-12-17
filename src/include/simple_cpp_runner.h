#ifndef SIMPLE_CPP_RUNNER_H
#define SIMPLE_CPP_RUNNER_H

#include "runner.h"
#include "simple_cpp_compiler.h"

namespace bjudger
{
class SimpleCppRunner : public Runnner
{
  private:
    SimpleCppCompiler compiler;
  public:

};

} // namespace bjudger

#endif // SIMPLE_CPP_RUNNER_H