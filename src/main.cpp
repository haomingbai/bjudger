#include "judger.h"
#include "problem.h"
#include <boost/json.hpp>
#include <boost/program_options.hpp>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

std::map<std::string, std::unique_ptr<bjudger::Problem>> problems;

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
        "config", po::value<std::string>()->default_value("/etc/bjudger/config.json"),
        "set the path of the configuration file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    // Load the configuration file
    std::ifstream configFile(vm["config"].as<std::string>());
    if (!configFile.is_open())
    {
        std::cerr << "Unable to open file." << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << configFile.rdbuf();
    std::string json_str = buffer.str();

    // Get the configuration
    boost::json::value json = boost::json::parse(json_str);

    boost::json::object config = json.as_object();
    size_t compilerNum = config.at("compilerNum").as_uint64();
    size_t threadNum = config.at("thread").as_uint64();
    size_t port = config.at("port").as_uint64();
    boost::json::array problems = config.at("problems").as_array();
    for (auto &problem : problems)
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
            std::string name = judger.as_object().at("name").as_string().c_str();
            std::string judgerPath = judger.as_object().at("path").as_string().c_str();
            std::string workingDirectory = judger.as_object().at("workingDirectory").as_string().c_str();
            size_t timeLimit = judger.as_object().at("timeLimit").as_uint64();
            size_t memoryLimit = judger.as_object().at("memoryLimit").as_uint64();
            size_t compilerNum = judger.as_object().at("compilerNum").as_uint64();
            std::string compilerPath = "";
            if (judger.as_object().contains("compilerPath") && judger.as_object().at("compilerPath").is_string())
            {
                compilerPath = judger.as_object().at("compilerPath").as_string().c_str();
            }
            std::string bsdbxPath = "";
            if (judger.as_object().contains("bsdbxPath") && judger.as_object().at("bsdbxPath").is_string())
            {
                bsdbxPath = judger.as_object().at("bsdbxPath").as_string().c_str();
            }
            std::string specialJudgerPath = "";
            if (judger.as_object().contains("specialJudgerPath") &&
                judger.as_object().at("specialJudgerPath").is_string())
            {
                specialJudgerPath = judger.as_object().at("specialJudgerPath").as_string().c_str();
            }
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
                             const_cast<char *>(compilerPath.c_str()), const_cast<char *>(bsdbxPath.c_str()),
                             compilerNum, timeLimit, memoryLimit, const_cast<char *>(specialJudgerPath.c_str())));
            problemPtr->addJudger(name, std::move(judgerPtr));
        }
    }
}
