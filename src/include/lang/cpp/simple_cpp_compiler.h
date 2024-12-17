#ifndef SIMPLE_CPP_COMPILER_H
#define SIMPLE_CPP_COMPILER_H

#include "simple_compiler.h"

namespace bjudger
{
class SimpleCppCompiler : public SimpleCompiler
{
  public:
    /**
     * @brief Constructs a SimpleCCompiler object.
     *
     * @param outputDirectory The directory where the compiled output will be stored.
     * @param compilerPath The path to the C compiler executable.
     * @param bsdbxPath The path to the sandbox executable.
     */
    SimpleCppCompiler(std::string outputDirectory, std::string compilerPath, std::string bsdbxPath);

    /**
     * @brief Compiles the given source code.
     *
     * This function takes a C++ source code string view and compiles it, returning the
     * path of the resulting binary as a string. The compilation is done using the
     *
     * @param src The source code to be compiled.
     * @return A string containing the compiled output.
     */
    virtual CompilerLog compile(std::string src) override;

    /**
     * @brief Compiles the given source code with a time limitation.
     *
     * This function takes a C++ source code string view and compiles it within the given
     * time limitation, returning the path of the resulting binary as a string. The unit
     * of the time limitation is miliseconds.
     *
     * @param src The source code to be compiled.
     * @param timeLimitation The time limitation for the compilation in miliseconds.
     * @return A string containing the compiled output.
     */
    virtual CompilerLog compile(std::string src, size_t timeLimitation) override;

    /**
     * @brief Compiles the given source code with the specified arguments.
     *
     * @param src The source code to compile.
     * @param argList A list of arguments to pass to the compiler.
     * @return CompilerLog The log generated during the compilation process.
     */
    virtual CompilerLog compile(std::string src, std::vector<std::string> argList) override;

    /**
     * @brief Compiles the given source code with specified arguments and time limitation.
     *
     * @param src The source code to compile.
     * @param argList A list of arguments to pass to the compiler.
     * @param timeLimitation The maximum time allowed for the compilation process.
     * @return CompilerLog The log generated during the compilation process.
     */
    virtual CompilerLog compile(std::string src, std::vector<std::string> argList, size_t timeLimitation) override;
};

} // namespace bjudger

#endif