#ifndef SIMPLE_COMPILER_H
#define SIMPLE_COMPILER_H

#include "compiler.h"
#include <mutex>

namespace bjudger
{
class SimpleCompiler : Compiler
{
  protected:
    std::mutex mtx;
    std::string outputDirectory;
    std::string compilerPath;
    std::string bsdbxPath;
};

} // namespace bjudger
#endif // SIMPLE_COMPILER_H