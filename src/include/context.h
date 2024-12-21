#include "problem.h"
#include <unordered_map>

namespace bjudger
{
struct Context
{
    std::unordered_map<std::string, std::unique_ptr<Problem>> problems;
};

} // namespace bjudger
