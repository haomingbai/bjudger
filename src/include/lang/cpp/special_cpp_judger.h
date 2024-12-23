#include "judger.h"
#include <mutex>
#include <memory>
#include "lang/cpp/simple_cpp_compiler.h"

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
  public:
    SpecialCppJudger(std::string workingDirectory, int compilerNum, size_t timeLimit, size_t memoryLimit, std::string specialJudgerPath);
    virtual JudgeResult judge(std::string code, std::vector<std::string> &input, std::vector<std::string> &expectedOutput) override;
    virtual JudgeResult judge(std::string code) override;
};
} // namespace bjudger