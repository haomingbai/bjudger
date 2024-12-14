#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <string_view>

namespace bjudger
{
struct CompilerLog;

class Compiler
{
  public:
    /**
     * @brief Compiles the given source code and returns the path of the binary.
     *
     * This pure virtual function takes a string containing source code and
     * compiles it, returning the path of the resulting binary as a string.
     * The exact nature of the compilation and the format of the returned
     * string depend on the implementation in derived classes.
     *
     * @param src The source code to be compiled.
     * @return A string containing the path of the resulting binary.
     */
    virtual CompilerLog compile(std::string_view src) = 0;
};

struct CompilerLog
{
  public:
    std::string path, log;
};

} // namespace bjudger

#endif // COMPILER_H