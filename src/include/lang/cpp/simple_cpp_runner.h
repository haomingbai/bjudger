#ifndef SIMPLE_CPP_RUNNER_H
#define SIMPLE_CPP_RUNNER_H

#include "lang/cpp/simple_cpp_compiler.h"
#include "runner.h"
#include <memory>

namespace bjudger
{
class SimpleCppRunner : public Runner
{
  private:
    std::unique_ptr<SimpleCppCompiler> compiler;
    std::string bsdbxPath;

  public:
    /**
     * @brief Constructs a SimpleCppRunner object.
     *
     * @param outputDirectory The directory where the output files will be stored.
     * @param compilerPath The path to the C++ compiler executable.
     * @param bsdbxPath The path to the sandbox executable.
     */
    SimpleCppRunner(std::string outputDirectory, std::string compilerPath, std::string bsdbxPath);

    virtual RunResult run(std::string code) override;
    virtual std::vector<RunResult> run(std::string code, std::vector<std::string> &input) override;
    virtual std::vector<RunResult> run(std::string code, std::vector<std::string> &input, size_t timeLimit,
                                       size_t memoryLimit) override;
};

} // namespace bjudger

#endif // SIMPLE_CPP_RUNNER_H