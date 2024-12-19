#include "problem.h"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace bjudger
{
namespace __internal_func__
{

std::string hash_to_hex_string(const std::vector<unsigned char> &hash);

bool compute_file_sha512(const std::string &path, std::vector<unsigned char> &hash_output);

std::string calculateFileSHA512(const std::string &path)
{
    std::vector<u_char> hash;
    bool flag = compute_file_sha512(path, hash);
    if (!flag)
    {
        // If fail to gen the sha, throw an exception.
        // Usually, the program should directly exit for no code to catch the exception.
        throw std::runtime_error("Failed to compute SHA512 hash");
    }

    return hash_to_hex_string(hash);
}

std::string hash_to_hex_string(const std::vector<unsigned char> &hash)
{
    std::string hex_string;
    for (unsigned char byte : hash)
    {
        char buf[3]; // Convert every byte to a string of len 2
        snprintf(buf, sizeof(buf), "%02x", byte);
        hex_string.append(buf);
    }
    return hex_string;
}

// Generated by bing copilot
bool compute_file_sha512(const std::string &path, std::vector<unsigned char> &hash_output)
{
    const size_t buffer_size = 4096; // 4KB buffer
    unsigned char buffer[buffer_size];

    // Open file
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    // Create EVP_MD_CTX object
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx)
    {
        return false;
    }

    // Initialize SHA512 context
    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), nullptr))
    {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    // Loop through the file and update the hash
    while (file.read(reinterpret_cast<char *>(buffer), buffer_size) || file.gcount() > 0)
    {
        if (1 != EVP_DigestUpdate(mdctx, buffer, file.gcount()))
        {
            EVP_MD_CTX_free(mdctx);
            return false;
        }
    }

    // Calculate the final hash
    hash_output.resize(SHA512_DIGEST_LENGTH);
    unsigned int hash_length = 0;
    if (1 != EVP_DigestFinal_ex(mdctx, hash_output.data(), &hash_length))
    {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    // Free the EVP_MD_CTX object
    EVP_MD_CTX_free(mdctx);

    return true;
}

// Generated by bing copilot
std::string calculateStringSHA512(const std::string &input)
{
    // Create EVP_MD_CTX object
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx)
    {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize SHA512 context
    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), nullptr))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to initialize digest context");
    }

    // Update the digest with the input string
    if (1 != EVP_DigestUpdate(mdctx, input.data(), input.size()))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to update digest");
    }

    // Finalize the digest
    std::vector<unsigned char> hash(SHA512_DIGEST_LENGTH);
    unsigned int hash_length = 0;
    if (1 != EVP_DigestFinal_ex(mdctx, hash.data(), &hash_length))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to finalize digest");
    }

    EVP_MD_CTX_free(mdctx); // Free the EVP_MD_CTX object

    // Convert the hash to a hex string
    std::string hex_string;
    for (unsigned char byte : hash)
    {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", byte);
        hex_string.append(buf);
    }

    return hex_string;
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

bool Problem::hasLang(std::string lang)
{
    return this->judgers.count(lang) > 0;
}

} // namespace bjudger
