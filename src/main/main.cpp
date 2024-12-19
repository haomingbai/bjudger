#include "problem.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream> // debug
#include <stdexcept>

void readConfig(std::string &json_str);

int main(int argc, char **argv)
{
//    namespace po = boost::program_options;
//    po::options_description desc("Allowed options");
//    desc.add_options()("help", "produce help message")(
//        "config", po::value<std::string>()->default_value("/etc/bjudger/config.json"),
//        "set the path of the configuration file");
//
//    po::variables_map vm;
//    po::store(po::parse_command_line(argc, argv, desc), vm);
//
//    // Load the configuration file
//    std::ifstream configFile(vm["config"].as<std::string>());
//    if (!configFile.is_open())
//    {
//        throw std::runtime_error("Unable to open configuration file");
//    }
//    std::stringstream buffer;
//    buffer << configFile.rdbuf();
//    std::string json_str = buffer.str();
    std::string json_str = "";

    json_str = R"(
{
    "thread": 4,
    "port": 8080,
    "problems": [
        {
            "path": "/home/haomingbai/problem",
            "specialJudge": false,
            "judgers": [
                {
                    "name": "cpp",
                    "path": "/home/haomingbai/bjudger/build/lib/libsimple_cpp_judger.so",
                    "workingDirectory": "/home/haomingbai/a",
                    "compilerPath": "/bin/gcc",
                    "runnerPath": "",
                    "bsdbxPath": "/bin/bsdbx",
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
    // Get the configuration
    readConfig(json_str);
    
}