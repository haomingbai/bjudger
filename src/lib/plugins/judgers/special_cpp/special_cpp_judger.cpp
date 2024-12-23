#include "lang/cpp/special_cpp_judger.h"

namespace bjudger
{
SpecialCppJudger::SpecialCppJudger(std::string workingDirectory, int compilerNum, size_t timeLimit, size_t memoryLimit, std::string specialJudgerPath)
{
}
} // namespace bjudger

extern "C"
{
    bjudger::Judger *createJudger(char *workingDirectory, char *runnerPath, char *compilerPath, char *bsdbxPath,
                                  int runnerNum, size_t timeLimit, size_t memoryLimit, char *specialJudgerPath);
}

bjudger::Judger *createJudger(char *workingDirectory, char *runnerPath, char *compilerPath, char *bsdbxPath,
                              int runnerNum, size_t timeLimit, size_t memoryLimit, char *specialJudgerPath);