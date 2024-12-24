#include "judger.h"
#include "lang/cpp/simple_cpp_compiler.h"
#include <memory>
#include <mutex>

namespace bjudger
{
class SpecialCppJudger : public Judger
{
  private:
    size_t timeLimit;
    size_t memoryLimit;
    std::mutex mtx;
    std::vector<std::unique_ptr<SimpleCppCompiler>> compilers;
    volatile size_t position = 0;
    std::string bsdbxPath;
    std::string specialJudgerPath;

  public:
    SpecialCppJudger(std::string workingDirectory, std::string compilerPath, int compilerNum, size_t timeLimit,
                     size_t memoryLimit, std::string specialJudgerPath, std::string bsdbxPath);
    virtual JudgeResult judge(std::string code, std::vector<std::string> &input,
                              std::vector<std::string> &expectedOutput)
    {
        return {{}, "", {}, {}};
    } // Not implemented
    virtual JudgeResult judge(std::string code) override;
};
} // namespace bjudger