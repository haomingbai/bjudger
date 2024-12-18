#include "problem.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <openssl/sha.h>
#include <sstream>
#include <memory>
#include <stdexcept>

namespace bjudger
{
namespace __internal_func__
{
std::string calculateFileSHA512(const std::string &file_path)
{
    // Open the file
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file");
    }

    // Initialize the SHA512 context
    SHA512_CTX sha512;
    SHA512_Init(&sha512);

    // Read the file and update the SHA512 context
    char buffer[4096];
    while (file.good())
    {
        file.read(buffer, sizeof(buffer));
        SHA512_Update(&sha512, buffer, file.gcount());
    }

    // Calculate the SHA512 hash
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_Final(hash, &sha512);

    // Convert the hash to a string
    std::stringstream ss;
    for (unsigned char c : hash)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }

    // Return the hash
    return ss.str();
}

std::string calculateStringSHA512(const std::string &input)
{
    // Initialize the SHA512 context
    SHA512_CTX sha512;
    SHA512_Init(&sha512);

    // Update the SHA512 context
    SHA512_Update(&sha512, input.c_str(), input.size());

    // Calculate the SHA512 hash
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_Final(hash, &sha512);

    // Convert the hash to a string
    std::stringstream ss;
    for (unsigned char c : hash)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }

    return ss.str();
}

std::string calculateBasicProblemId(std::string &path)
{
    namespace fs = std::filesystem;
    fs::path problemDirectory(path);
    if (!fs::exists(problemDirectory) || !fs::is_directory(problemDirectory))
    {
        throw std::invalid_argument("The path is not a directory");
    }

    // Get the list of input and output files
    std::vector<fs::path> inputFiles;
    std::vector<fs::path> outputFiles;
    for (const auto &entry : std::filesystem::directory_iterator(problemDirectory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".in")
        {
            inputFiles.emplace_back(entry.path());
        }
        if (entry.is_regular_file() && entry.path().extension() == ".out")
        {
            outputFiles.emplace_back(entry.path());
        }
    }
    std::sort(inputFiles.begin(), inputFiles.end(),
              [](const fs::path &a, const fs::path &b) { return a.filename().string() < b.filename().string(); });
    std::sort(outputFiles.begin(), outputFiles.end(),
              [](const fs::path &a, const fs::path &b) { return a.filename().string() < b.filename().string(); });

    std::string originalId = "";

    // Calculate the SHA of the context of the "content.md"
    std::string contentPath = problemDirectory / "content.md";
    if (fs::exists(contentPath))
    {
        originalId += calculateFileSHA512(contentPath);
    }

    // Calculate the SHA of *.in files
    for (const auto &inputFile : inputFiles)
    {
        originalId += calculateFileSHA512(problemDirectory / inputFile);
    }

    // Calculate the SHA of *.out files
    for (const auto &outputFile : outputFiles)
    {
        originalId += calculateFileSHA512(problemDirectory / outputFile);
    }

    return calculateStringSHA512(originalId);
}
} // namespace __internal_func__
Problem::Problem(std::string path, bool isSpecialJudge)
{
    this->isSpecialJudge = isSpecialJudge;

    // First check if the path is a directory and if the number of input and output files are the same
    namespace fs = std::filesystem;
    fs::path problemDirectory(path);
    if (!fs::exists(problemDirectory) || !fs::is_directory(problemDirectory))
    {
        throw std::invalid_argument("The path is not a directory");
    }

    size_t inputNum = 0;
    size_t outputNum = 0;
    for (const auto &entry : fs::directory_iterator(problemDirectory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".in")
        {
            inputNum++;
        }
        else if (entry.is_regular_file() && entry.path().extension() == ".out")
        {
            outputNum++;
        }
    }

    if (inputNum != outputNum)
    {
        throw std::invalid_argument("The number of input and output files are not the same");
    }

    // Calculate the basic problem id
    this->id = __internal_func__::calculateBasicProblemId(path);

    // Read the name of all input and output files without the extension
    std::vector<std::string> inputNames;
    for (const auto &entry : fs::directory_iterator(problemDirectory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".in")
        {
            inputNames.push_back(entry.path().filename().stem().string());
        }
    }

    // Sort the input names
    std::sort(inputNames.begin(), inputNames.end());

    // Read the content of the input and output files
    for (const auto &inputName : inputNames)
    {
        std::string inputPath = (problemDirectory / (inputName + ".in")).string();
        std::string outputPath = (problemDirectory / (inputName + ".out")).string();

        // Read the input file
        std::ifstream inputFile(inputPath);
        if (!inputFile.is_open())
        {
            throw std::runtime_error("Unable to open input file");
        }
        std::stringstream inputBuffer;
        inputBuffer << inputFile.rdbuf();
        this->inputs.push_back(inputBuffer.str());

        // Read the output file
        std::ifstream outputFile(outputPath);
        if (!outputFile.is_open())
        {
            throw std::runtime_error("Unable to open output file");
        }
        std::stringstream outputBuffer;
        outputBuffer << outputFile.rdbuf();
        this->outputs.push_back(outputBuffer.str());
    }
}

void Problem::addJudger(std::string lang, std::unique_ptr<Judger> judger)
{
    this->judgers[lang] = std::move(judger);
}

FinalResult Problem::judge(std::string code, std::string lang)
{
    if (this->judgers.find(lang) != this->judgers.end())
    {
        FinalResult result;
        result.isSuccessful = true;
        result.result = std::move(this->judgers[lang]->judge(code, this->inputs, this->outputs));
        return result;
    }
    else
    {
        FinalResult result;
        result.isSuccessful = false;
        return result;
    }
}

std::string Problem::getId()
{
    return this->id;
}

} // namespace bjudger
