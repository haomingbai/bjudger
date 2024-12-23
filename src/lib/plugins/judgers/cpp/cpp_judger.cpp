#include "lang/cpp/cpp_judger.h"
#include <algorithm>
#include <filesystem>
#include <iostream> // Debug
#include <ranges>

namespace bjudger
{

CppJudger::CppJudger(std::string workingDirectory, std::string compilerPath, std::string bsdbxPath, int runnerNum,
                     size_t timeLimit, size_t memoryLimit)
{
    this->timeLimit = timeLimit;
    this->memoryLimit = memoryLimit;
    std::filesystem::path workingDirectoryRoot(workingDirectory);
    if (!std::filesystem::exists(workingDirectoryRoot))
    {
        std::filesystem::create_directories(workingDirectoryRoot);
    }

    // Create the seperate working directories for each runner
    if (runnerNum <= 0)
    {
        throw std::invalid_argument("Runner number must be greater than 0");
    }

    for (int i = 0; i < runnerNum; i++)
    {
        std::filesystem::path runnerWorkingDirectory = workingDirectoryRoot / ("runner" + std::to_string(i));
        if (!std::filesystem::exists(runnerWorkingDirectory))
        {
            std::filesystem::create_directories(runnerWorkingDirectory);
        }
        runners.push_back(std::make_unique<SimpleCppRunner>(runnerWorkingDirectory.string(), compilerPath, bsdbxPath));
    }
}

JudgeResult CppJudger::judge(std::string code, std::vector<std::string> &input,
                             std::vector<std::string> &expectedOutput)
{
    // Make sure the input and expected output have the same size
    if (input.size() != expectedOutput.size())
    {
        throw std::invalid_argument("Input and expected output size mismatch");
    }

    // Make sure which compiler will be used
    std::unique_lock<std::mutex> lock(mtx);
    size_t pos = position;
    position = (position + 1) % runners.size();
    lock.unlock();

    // Run the code
    using namespace std;
    auto runResults = runners[pos]->run(move(code), input, timeLimit, memoryLimit);

    // Compare the output. if there is no error, then the memory usage and time usage will occur in the error message of
    // the result, which will display "MLE" and "TLE" respectively.

    // First detect whether is CE
    if (runResults[0].exitCode == -1)
    {
        return JudgeResult{{JudgeResult::CE}, runResults[0].error, {0}, {0}};
    }

    JudgeResult result; // The result to be returned
    result.status.reserve(input.size());
    result.memoryUsage.reserve(input.size());
    result.timeUsage.reserve(input.size());

    // Then iterate the results to check the output
    for (size_t i = 0; i < input.size(); i++)
    {
        // First slice the output and expected output with the range view in C++20
        // Expected output
        auto v = expectedOutput[i] | std::views::split('\n') |
                 std::views::transform([](auto line) { return std::string(line.begin(), line.end()); });
        vector<string> expectedOutputSplited(v.begin(), v.end());
        for (auto &s : expectedOutputSplited)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                    s.end());
        }
        while (expectedOutputSplited.size() > 0 && (expectedOutputSplited.back().length() == 0))
        {
            expectedOutputSplited.pop_back();
        }

        // Output
        auto v2 = runResults[i].output | std::views::split('\n') |
                  std::views::transform([](auto line) { return std::string(line.begin(), line.end()); });
        vector<string> outputSplited(v2.begin(), v2.end());
        for (auto &s : outputSplited)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                    s.end());
        }
        while (outputSplited.size() > 0 && (outputSplited.back().length() == 0))
        {
            outputSplited.pop_back();
        }

        // Fetch the last two lines of the error message
        auto errorSplited = runResults[i].error | std::views::split('\n') |
                            std::views::transform([](auto word) { return std::string(word.begin(), word.end()); });
        vector<string> errorSplitedVector(errorSplited.begin(), errorSplited.end());
        for (auto &s : errorSplitedVector)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                    s.end());
        }
        while (errorSplitedVector.size() > 0 && (errorSplitedVector.back().length() == 0))
        {
            errorSplitedVector.pop_back();
        }

        string memoryUsage = errorSplitedVector[errorSplitedVector.size() - 2];
        string timeUsage = errorSplitedVector[errorSplitedVector.size() - 1];

        auto end = std::find_if_not(memoryUsage.rbegin(), memoryUsage.rend(), [](unsigned char ch) {
                       return std::isspace(ch);
                   }).base();
        memoryUsage.erase(end, memoryUsage.end());

        end = std::find_if_not(timeUsage.rbegin(), timeUsage.rend(), [](unsigned char ch) {
                  return std::isspace(ch);
              }).base();
        timeUsage.erase(end, timeUsage.end());

        // Find if there is TLE and MLE
        if (memoryUsage.find("MLE") != string::npos)
        {
            result.status.push_back(JudgeResult::MLE);
            result.memoryUsage.push_back(memoryLimit);
            if (timeUsage.find("TLE") != string::npos)
            {
                result.timeUsage.push_back(timeLimit);
            }
            else
            {
                result.timeUsage.push_back(stoi(timeUsage));
            }
        }
        else if (timeUsage.find("TLE") != string::npos)
        {
            result.status.push_back(JudgeResult::TLE);
            result.memoryUsage.push_back(stoi(memoryUsage));
            result.timeUsage.push_back(timeLimit);
        }
        else if (runResults[i].exitCode != 0)
        {
            result.status.push_back(JudgeResult::RE);
            result.memoryUsage.push_back(stoi(memoryUsage));
            result.timeUsage.push_back(stoi(timeUsage));
        }
        else // Compare the result line by line
        {
            // Compare the size first
            if (expectedOutputSplited.size() != outputSplited.size())
            {
                result.status.push_back(JudgeResult::WA);
                result.memoryUsage.push_back(stoi(memoryUsage));
                result.timeUsage.push_back(stoi(timeUsage));
            }
            else // Compare the content
            {
                bool isWA = false; // The flag to indicate whether the output is WA
                for (size_t j = 0; j < expectedOutputSplited.size(); j++)
                {
                    // If the output is not the same, then it is WA
                    if (expectedOutputSplited[j] != outputSplited[j])
                    {
                        isWA = true;
                        break;
                    }
                }

                // Determine the result
                if (isWA)
                {
                    result.status.push_back(JudgeResult::WA);
                    result.memoryUsage.push_back(stoi(memoryUsage));
                    result.timeUsage.push_back(stoi(timeUsage));
                }
                else
                {
                    result.status.push_back(JudgeResult::AC);
                    result.memoryUsage.push_back(stoi(memoryUsage));
                    result.timeUsage.push_back(stoi(timeUsage));
                }
            }
        }
    }

    // Return the result
    return result;
}

JudgeResult CppJudger::judge(std::string code)
{
    // Currently, we do not support this function, but it is useful is a special judge is needed
    return JudgeResult{};
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
    return new bjudger::CppJudger(workingDirectory, compilerPath, bsdbxPath, runnerNum, timeLimit, memoryLimit);
}