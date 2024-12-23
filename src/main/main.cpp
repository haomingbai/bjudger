#include "context.h"
#include "problem.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream> // debug
#include <stdexcept>

void readConfig(std::string &json_str);
void initServer(int port, const std::string &cert, const std::string &key);

int main(int argc, char **argv)
{
#ifndef DEBUG
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
        throw std::runtime_error("Unable to open configuration file");
    }
    std::stringstream buffer;
    buffer << configFile.rdbuf();
    std::string json_str = buffer.str();
#endif

#ifdef DEBUG
    std::string json_str;
    json_str = R"(
{
    "thread": 100,
    "port": 8080,
    "apis": [
        {
            "name": "/exist",
            "path": "/home/haomingbai/bjudger/build/lib/libbjudger_api_exist.so"
        },
        {
            "name": "/judge",
            "path": "/home/haomingbai/bjudger/build/lib/libbjudger_api_judge.so"
        }
    ],
    "problems": [
        {
            "path": "/home/haomingbai/problem",
            "specialJudge": false,
            "judgers": [
                {
                    "name": "cpp",
                    "path": "/home/haomingbai/bjudger/build/lib/libbjudger_simple_cpp_judger.so",
                    "workingDirectory": "/home/haomingbai/a",
                    "compilerPath": "/bin/g++",
                    "runnerPath": "",
                    "bsdbxPath": "/home/haomingbai/bjudger/build/bin/bsdbx",
                    "specialJudgerPath": "",
                    "compilerNum": 4,
                    "memoryLimit": 1024,
                    "timeLimit": 1000
                }
            ]
        }
    ]
}
    )";
#endif
    // Get the configuration
    readConfig(json_str);
}