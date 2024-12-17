#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <string_view>
#include <vector>

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
    virtual CompilerLog compile(std::string src) = 0;

    /**
     * @brief Compiles the given source code within the specified time limitation.
     *
     * @param src The source code to be compiled, provided as a string view.
     * @param timeLimitation The maximum amount of time allowed for the compilation process, in miliseconds.
     * @return CompilerLog An object containing the log of the compilation process.
     */
    virtual CompilerLog compile(std::string src, size_t timeLimitation) = 0;

    /**
     * @brief Compiles the given source code with the specified arguments.
     *
     * This is a pure virtual function that must be implemented by derived classes.
     * It takes the source code as a string view and a vector of arguments to be
     * passed to the compiler.
     *
     * @param src The source code to be compiled.
     * @param argList A vector of arguments to be passed to the compiler.
     * @return A CompilerLog object containing the results of the compilation.
     */
    virtual CompilerLog compile(std::string src, std::vector<std::string> argList) = 0;

    /**
     * @brief Compiles the given source code with specified arguments and time limitation.
     *
     * @param src The source code to be compiled.
     * @param argList A list of arguments to be passed to the compiler.
     * @param timeLimitation The maximum time allowed for the compilation process, the unit of the time limitation is miliseconds.
     * @return CompilerLog The log generated during the compilation process.
     */
    virtual CompilerLog compile(std::string src, std::vector<std::string> argList, size_t timeLimitation) = 0;
};

struct CompilerLog
{
    std::string path, log;
};

} // namespace bjudger

#endif // COMPILER_H