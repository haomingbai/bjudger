#ifndef SIMPLE_C_COMPILER_H
#define SIMPLE_C_COMPILER_H

#include "simple_compiler.h"

namespace bjudger
{
class SimpleCCompiler : public SimpleCompiler
{
  public:
    /**
     * @brief Constructs a SimpleCCompiler object.
     *
     * @param outputDirectory The directory where the compiled output will be stored.
     * @param compilerPath The path to the C compiler executable.
     */
    SimpleCCompiler(std::string outputDirectory, std::string compilerPath, std::string bsdbxPath);

    /**
     * @brief Compiles the given source code.
     *
     * This function takes a C source code string view and compiles it, returning the
     * path of the resulting binary as a string. The compilation is done using the
     *
     * @param src The source code to be compiled.
     * @return A string containing the compiled output.
     */
    virtual CompilerLog compile(std::string_view src) override;
};
} // namespace bjudger

#endif // SIMPLE_C_COMPILER_H