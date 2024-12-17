#ifndef RUNNER_H
#define RUNNER_H

#include <string>

namespace bjudger
{

struct RunResult
{
    int exitCode;
    std::string output;
    std::string error;
};

class Runnner
{
  public:
    /**
     * @brief Executes the given code and returns the result.
     *
     * @param code The source code to be executed as a string.
     * @return RunResult The result of the code execution.
     */
    virtual RunResult run(std::string code) = 0;

    /**
     * @brief Executes the given code with the provided input.
     *
     * @param code The code to be executed as a string.
     * @param input The input to be provided to the code as a string.
     * @return RunResult The result of the code execution.
     */
    virtual RunResult run(std::string code, std::string input) = 0;

    /**
     * @brief Executes the given code with the specified input, time limit, and memory limit.
     *
     * @param code The source code to be executed.
     * @param input The input to be provided to the code during execution.
     * @param timeLimit The maximum amount of time (in milliseconds) the code is allowed to run.
     * @param memoryLimit The maximum amount of memory (in KB) the code is allowed to use.
     * @return RunResult The result of the code execution, including output, execution time, and memory usage.
     */
    virtual RunResult run(std::string code, std::string input, size_t timeLimit, size_t memoryLimit) = 0;
};

} // namespace bjudger

#endif // RUNNER_H