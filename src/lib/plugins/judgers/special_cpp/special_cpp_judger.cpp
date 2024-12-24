#include "lang/cpp/special_cpp_judger.h"
#include <dlfcn.h>
#include <filesystem>
#include <unistd.h>

namespace bjudger
{
SpecialCppJudger::SpecialCppJudger(std::string workingDirectory, std::string compilerPath, int compilerNum,
                                   size_t timeLimit, size_t memoryLimit, std::string specialJudgerPath,
                                   std::string bsdbxPath)
{
    this->memoryLimit = memoryLimit;
    this->timeLimit = timeLimit;

    for (size_t i = 0; i < compilerNum; i++)
    {
        namespace fs = std::filesystem;
        fs::path dir = fs::path(workingDirectory) / ("runnner" + std::to_string(i));
        if (!fs::exists(dir))
        {
            fs::create_directories(dir);
        }
        this->compilers.push_back(std::make_unique<SimpleCppCompiler>(dir.string(), compilerPath, bsdbxPath));
    }

    this->specialJudgerPath = specialJudgerPath;
    this->bsdbxPath = bsdbxPath;
}

JudgeResult SpecialCppJudger::judge(std::string code)
{
    // Determine which compiler to use
    std::unique_lock<std::mutex> lock(this->mtx);
    auto position = this->position;
    this->position = (this->position + 1) % this->compilers.size();
    lock.unlock();

    // Compile the code
    auto compiler = this->compilers[position].get();
    auto compilerLog = compiler->compile(code);
    if (compilerLog.log.empty())
    {
        return {{JudgeResult::CE}, compilerLog.log, {0}, {0}};
    }

    // load the function
    auto handler = dlopen(this->specialJudgerPath.c_str(), RTLD_LAZY);
    auto func = (SimpleJudgeResult(*)(const char *, int, int))dlsym(handler, "judge");
    if (!func)
    {
        return {{JudgeResult::RE}, "Error loading function", {0}, {0}};
    }

    // Run the special judger
    auto result = func(compilerLog.path.c_str(), this->timeLimit, this->memoryLimit);
    JudgeResult judgeResult{
        {static_cast<JudgeResult::Status>(result.status)}, compilerLog.log, {result.timeUsage}, {result.memoryUsage}};

    // Close the handler
    dlclose(handler);
    return judgeResult;
}
} // namespace bjudger

extern "C"
{
    bjudger::Judger *createJudger(char *workingDirectory, char *runnerPath, char *compilerPath, char *bsdbxPath,
                                  int runnerNum, size_t timeLimit, size_t memoryLimit, char *specialJudgerPath);
}

bjudger::Judger *createJudger(char *workingDirectory, char *runnerPath, char *compilerPath, char *bsdbxPath,
                              int runnerNum, size_t timeLimit, size_t memoryLimit, char *specialJudgerPath)
{
    return new bjudger::SpecialCppJudger(workingDirectory, compilerPath, runnerNum, timeLimit, memoryLimit,
                                         specialJudgerPath, bsdbxPath);
}