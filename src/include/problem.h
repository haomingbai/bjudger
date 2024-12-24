#ifndef PROBLEM_H
#define PROBLEM_H

#include "judger.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace bjudger
{
struct FinalResult
{
    bool isSuccessful;
    JudgeResult result;
};

class Problem
{
  protected:
    bool isSpecialJudge;
    std::unordered_map<std::string, std::unique_ptr<Judger>> judgers;
    std::string id;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::string specialJudgerPath;
    std::string problemName;

  public:
    /**
     * @brief Construct a new Problem object
     *
     * @param path The path to the problem
     * @param isSpecialJudge Whether the problem needs a special judge
     */
    Problem(std::string path, bool isSpecialJudge);

    /**
     * @brief Judges the provided code in the specified programming language.
     *
     * @param code The source code to be judged.
     * @param lang The programming language of the source code.
     * @return FinalResult The result of the judging process.
     */
    FinalResult judge(std::string code, std::string lang);

    /**
     * @brief Retrieves the unique identifier of the problem.
     *
     * @return A string representing the unique identifier of the problem.
     */
    std::string getId();

    /**
     * @brief Adds a Judger to the problem for a specific programming language.
     *
     * @param lang The programming language for which the Judger is being added.
     * @param judger A unique pointer to the Judger object to be added.
     */
    void addJudger(std::string lang, std::unique_ptr<Judger> judger);

    /**
     * @brief Checks if the problem supports a given programming language.
     *
     * @param lang The programming language to check for support.
     * @return true if the language is supported, false otherwise.
     */
    bool hasLang(std::string lang);

    /**
     * @brief Retrieves the path to the special judger.
     *
     * This function returns the file path where the special judger is located.
     * The special judger is used for custom judging logic in the problem.
     *
     * @return std::string The path to the special judger.
     */
    std::string getSpecialJudgerPath();

    std::string getProblemName();

    virtual ~Problem() = default;
};
} // namespace bjudger

#endif // PROBLEM_H