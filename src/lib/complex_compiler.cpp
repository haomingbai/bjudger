#include "lang/complex_compiler.h"
#include <exception>
#include <stdexcept>

namespace bjudger
{
    CompilerLog ComplexCompiler::compile(std::string src, std::vector<std::string> argList, size_t timeLimitation)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (compilers.empty())
        {
            throw std::runtime_error("No compilers added to the complex compiler.");
        }
        auto current = position.fetch_add(1);
        position = position % compilers.size();
        lock.unlock();
        return compilers[current]->compile(src, argList, timeLimitation);
    }

    CompilerLog ComplexCompiler::compile(std::string src, size_t timeLimitation)
    {
        std::vector<std::string> argList = {};
        return compile(src, argList, timeLimitation);
    }

    CompilerLog ComplexCompiler::compile(std::string src, std::vector<std::string> argList)
    {
        return compile(src, argList, 1e9);
    }

    CompilerLog ComplexCompiler::compile(std::string src)
    {
        std::vector<std::string> argList = {};
        return compile(src, argList, 1e9);
    }
} // namespace bjudger