#include "lang/cpp/simple_cpp_runner.h"
#include <boost/process/v2.hpp>
#include <string>
#include <vector>
#include <iostream> // Debug

namespace bjudger
{
std::vector<RunResult> SimpleCppRunner::run(std::string code, std::vector<std::string> &input, size_t timeLimit,
                                            size_t memoryLimit)
{
    // Compile the code
    auto compilerResult = compiler->compile(std::move(code));
    auto path = compilerResult.path;
    auto log = compilerResult.log;

    if (path.empty())
    {
        return std::vector({RunResult{-1, "", std::move(log)}});
    }

    // Run the code
    namespace bp = boost::process;
    namespace asio = boost::asio;
    std::vector<RunResult> results;
    results.reserve(input.size());

    for (auto &input : input)
    {
        asio::io_context ctx;
        asio::readable_pipe out{ctx};
        asio::readable_pipe err{ctx};
        asio::writable_pipe in{ctx};
        boost::system::error_code ret;

        using namespace std;
        // Prepare the arguments for the sandbox
        vector<string> args = {path};
        args.push_back("--mode=runner");
        args.push_back("--time-limit=" + to_string(timeLimit));
        args.push_back("--memory-limit=" + to_string(memoryLimit));

        // Create the sandbox process
        bp::process runnerProcess(ctx, this->bsdbxPath, args, bp::process_stdio{{in}, {out}, {err}});
        in.write_some(asio::buffer(input));
        in.close();
        string output, log;

        // Fetch results
        auto exitCode = runnerProcess.wait(ret);
        RunResult result;

        // Store the exit code
        result.exitCode = exitCode;

        string tmp(1024, '\0');
        boost::system::error_code ec;
        int size = 0;
        while (size = out.read_some(asio::buffer(tmp, 1000), ec))
        {
            // cout << "Reading output: " << tmp << endl;
            tmp.resize(size);
            output += tmp;
        }
        while (size = err.read_some(asio::buffer(tmp, 1000), ec))
        {
            // cout << "Reading error: " << tmp << endl;
            tmp.resize(size);
            log += tmp;
        }

        // Close the pipes
        out.close();
        err.close();

        // Store the results
        result.output = move(output);
        result.error = move(log);
        results.push_back(result);
    }

    // Return the results
    return results;
}

SimpleCppRunner::SimpleCppRunner(std::string outputDirectory, std::string compilerPath, std::string bsdbxPath)
{
    compiler = std::make_unique<SimpleCppCompiler>(outputDirectory, compilerPath, bsdbxPath);
    this->bsdbxPath = bsdbxPath;
}

RunResult SimpleCppRunner::run(std::string code)
{
    std::vector<std::string> input = {""};
    return run(code, input, 0, 0).at(0);
}

std::vector<RunResult> SimpleCppRunner::run(std::string code, std::vector<std::string> &input)
{
    return run(code, (input), 0, 0);
}
} // namespace bjudger
