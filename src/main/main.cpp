#include "problem.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

extern std::map<std::string, std::unique_ptr<bjudger::Problem>> problems;

void readConfig(std::string &json_str);

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
        throw std::runtime_error("Unable to open configuration file");
    }
    std::stringstream buffer;
    buffer << configFile.rdbuf();
    std::string json_str = buffer.str();

    // Get the configuration
    readConfig(json_str);
}