#include "simple_c_compiler.h"
#include <boost/asio.hpp>
#include <boost/process/v2.hpp>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <string>
#include <iostream>

namespace bjudger
{
SimpleCCompiler::SimpleCCompiler(std::string outputDirectory, std::string compilerPath, std::string bsdbxPath)
{
    using namespace std;
    namespace fs = filesystem;
    if (!fs::exists(outputDirectory))
    {
        fs::create_directories(outputDirectory);
    }
    this->outputDirectory = outputDirectory;
    auto exists = fs::exists(compilerPath) && (!fs::is_directory(compilerPath)) && fs::exists(bsdbxPath) &&
                  (!fs::is_directory(bsdbxPath));
    if (!exists)
    {
        throw invalid_argument("Invalid compiler path or bsdbx path.");
    }
    else
    {
        this->compilerPath = compilerPath;
        this->bsdbxPath = bsdbxPath;
    }
}
CompilerLog SimpleCCompiler::compile(std::string_view src)
{
    using namespace std;
    namespace bp = boost::process;

    // Add the lock to synchronize access to this function.
    lock_guard<mutex> lock(this->mtx);

    // Generate the path of t<ostream>he output file.
    auto timestamp = chrono::system_clock::now().time_since_epoch().count();
    string outputFilePath = outputDirectory + '/' + to_string(timestamp) + ".out";

    // Write the source code to a temporary file.
    string tempFilePath = outputDirectory + '/' + to_string(timestamp) + ".c";
    ofstream tempFile(tempFilePath);
    tempFile << src;
    tempFile.close();

    // Generate the command and arguments.
    auto command = this->bsdbxPath;
    vector<string> args = {};
    args.push_back("--path=" + compilerPath);
    args.push_back("--mode=compiler");
    args.push_back(tempFilePath);
    args.push_back("-o");
    args.push_back(outputFilePath);

    // Run the compiler.
    boost::asio::io_context ctx;
    boost::asio::readable_pipe err{ctx};
    boost::system::error_code ret;
    int code;
    try
    {
        bp::process compilerProcess(ctx, command, args, bp::process_stdio{{}, {}, err});
        code = compilerProcess.wait(ret);
    }
    catch (std::exception e)
    {
        return {"", e.what()};
    }

    if (!ret.failed() && code == 0)
    {
        string log;
        boost::system::error_code ec;
        err.read_some(boost::asio::buffer(log), ec);
        return {outputFilePath, log};
    }
    else
    {
        string log;
        log.resize(1024);
        boost::system::error_code ec;
        err.read_some(boost::asio::buffer(log), ec);
        log.shrink_to_fit();
        return {"", log};
    }
}
} // namespace bjudger
