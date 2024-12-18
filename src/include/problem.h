#ifndef PROBLEM_H
#define PROBLEM_H

#include "judger.h"
#include <map>
#include <string>
#include <memory>

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
    std::map<std::string, std::unique_ptr<Judger>> judgers;
    std::string id;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;

  public:
    /**
     * @brief Construct a new Problem object
     *
     * @param path The path to the problem
     * @param isSpecialJudge Whether the problem needs a special judge
     */
    Problem(std::string path, bool isSpecialJudge);

    FinalResult judge(std::string code, std::string lang);

    std::string getId();

    void addJudger(std::string lang, std::unique_ptr<Judger> judger);
};
} // namespace bjudger

#endif // PROBLEM_H