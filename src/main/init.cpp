#include "judger.h"
#include "problem.h"
#include <boost/json.hpp>
#include <dlfcn.h>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <iostream> // debug

extern std::unordered_map<std::string, std::unique_ptr<bjudger::Problem>> problems;

void readConfig(std::string &json_str);
void loadProblem(boost::json::value &problem);

void readConfig(std::string &json_str)
{
    boost::json::value json = boost::json::parse(json_str);

    boost::json::object config = json.as_object();
    size_t threadNum = config.at("thread").as_int64();
    size_t port = config.at("port").as_int64();
    boost::json::array problemList = config.at("problems").as_array();
    for (auto &problem : problemList)
    {
        loadProblem(problem);
    }
}

void loadProblem(boost::json::value &problem)
{
    auto problemObj = problem.as_object();
    std::string path = problemObj.at("path").as_string().c_str();
    bool isSpecial = problemObj.at("specialJudge").as_bool();

    // Create the problem object
    auto problemPtr = std::make_unique<bjudger::Problem>(path, isSpecial);
    auto problemId = problemPtr->getId();

    // Add the judgers
    auto judgers = problemObj.at("judgers").as_array();

    for (auto &judger : judgers)
    {
        // Get necessary information

        // Name, the path of the judger's library, working directory and number of compilers are necessary.
        std::string name = judger.as_object().at("name").as_string().c_str();
        std::string judgerPath = judger.as_object().at("path").as_string().c_str();
        std::string workingDirectory = judger.as_object().at("workingDirectory").as_string().c_str();
        size_t compilerNum = judger.as_object().at("compilerNum").as_int64();

        // Time limit and memory limit are not necessary, but they are used in most cases, so in this function, they are
        // necessary. If you don't need them, just fill them with 0.
        size_t timeLimit = judger.as_object().at("timeLimit").as_int64();
        size_t memoryLimit = judger.as_object().at("memoryLimit").as_int64();

        // The path of the compiler, bsdbx, special judger and runner are not necessary, which means the it depends on
        // the realization of the judger. If they do not exist, the program will fill them with empty string.

        // The path of compiler.
        std::string compilerPath = "";
        if (judger.as_object().contains("compilerPath") && judger.as_object().at("compilerPath").is_string())
        {
            compilerPath = judger.as_object().at("compilerPath").as_string().c_str();
        }

        // The path of bsdbx.
        std::string bsdbxPath = "";
        if (judger.as_object().contains("bsdbxPath") && judger.as_object().at("bsdbxPath").is_string())
        {
            bsdbxPath = judger.as_object().at("bsdbxPath").as_string().c_str();
        }

        // The path of special judger.
        std::string specialJudgerPath = "";
        if (judger.as_object().contains("specialJudgerPath") && judger.as_object().at("specialJudgerPath").is_string())
        {
            specialJudgerPath = judger.as_object().at("specialJudgerPath").as_string().c_str();
        }

        // The path of runner.
        std::string runnerPath = "";
        if (judger.as_object().contains("runnerPath") && judger.as_object().at("runnerPath").is_string())
        {
            runnerPath = judger.as_object().at("runnerPath").as_string().c_str();
        }

        // Load the library
        auto handler = dlopen(judgerPath.c_str(), RTLD_LAZY);
        if (!handler)
        {
            perror("Error loading judger");
            throw std::runtime_error("Error loading judger library");
        }

        // Get the createJudger function
        auto createJudger = (bjudger::Judger * (*)(char *, char *, char *, char *, int, size_t, size_t, char *))
            dlsym(handler, "createJudger");
        if (!createJudger)
        {
            perror("Error loading createJudger");
            throw std::runtime_error("Error loading createJudger");
        }

        // Create the judger
        std::unique_ptr<bjudger::Judger> judgerPtr(
            createJudger(const_cast<char *>(workingDirectory.c_str()), const_cast<char *>(runnerPath.c_str()),
                         const_cast<char *>(compilerPath.c_str()), const_cast<char *>(bsdbxPath.c_str()), compilerNum,
                         timeLimit, memoryLimit, const_cast<char *>(specialJudgerPath.c_str())));
        problemPtr->addJudger(name, std::move(judgerPtr));
    }

    // Add the problem to the map
    problems[problemId] = std::move(problemPtr);
}