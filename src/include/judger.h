#ifndef JUDGER_H
#define JUDGER_H

#include <string>
#include <vector>

struct SimpleJudgeResult
{
    size_t timeUsage;
    size_t memoryUsage;
    int status;
};

namespace bjudger
{
struct JudgeResult
{
    enum Status
    {
        AC,
        WA,
        TLE,
        MLE,
        RE,
        CE
    };
    std::vector<Status> status;
    std::string compilerLog;
    std::vector<size_t> timeUsage;
    std::vector<size_t> memoryUsage;
};

class Judger
{
  public:
    /**
     * @brief Judge the given code against the provided inputs and expected outputs.
     *
     * @param code The source code to be judged.
     * @param input A vector of input strings to be fed to the code.
     * @param expectedOutput A vector of expected output strings to be compared against the code's output.
     * @param timeLimit The maximum allowed execution time for the code (in milliseconds).
     * @param memoryLimit The maximum allowed memory usage for the code (in bytes).
     * @return JudgeResult The result of the judging process, indicating success, failure, or any errors encountered.
     */
    virtual JudgeResult judge(std::string code, std::vector<std::string> &input,
                              std::vector<std::string> &expectedOutput) = 0;

    /**
     * @brief Pure virtual function to judge the provided code.
     *
     * This function takes a string containing the code to be judged and returns a JudgeResult object.
     *
     * @param code A string containing the code to be judged.
     * @return JudgeResult The result of the judging process.
     */
    virtual JudgeResult judge(std::string code) = 0;

    virtual ~Judger() = default;
};

} // namespace bjudger

extern "C"
{
    bjudger::Judger *createJudger(char *workingDirectory, char *runnerPath, char *compilerPath, char *bsdbxPath,
                                  int runnerNum, size_t timeLimit, size_t memoryLimit, char *specialJudgerPath);
}

#endif // JUDGER_H