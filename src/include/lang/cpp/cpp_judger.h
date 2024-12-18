#ifndef LANG_CPP_CPP_JUDGER_H
#define LANG_CPP_CPP_JUDGER_H

#include "judger.h"
#include "simple_cpp_runner.h"
#include <memory>
#include <mutex>
#include <vector>

namespace bjudger
{
class CppJudger : public Judger
{
  protected:
    std::vector<std::unique_ptr<SimpleCppRunner>> runners;
    volatile size_t position = 0;
    std::mutex mtx;
    size_t timeLimit;
    size_t memoryLimit;

  public:
    virtual JudgeResult judge(std::string code, std::vector<std::string> &input,
                              std::vector<std::string> &expectedOutput) override;
    virtual JudgeResult judge(std::string code) override;

    CppJudger(std::string workingDirectory, std::string compilerPath, std::string bsdbxPath, int runnerNum, size_t timelimit, size_t memorylimit);
};
} // namespace bjudger

#endif // LANG_CPP_CPP_JUDGER_H