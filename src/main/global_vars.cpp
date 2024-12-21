#include <unordered_map>
#include <memory>
#include <string>
#include "problem.h"
#include "context.h"
#include <unordered_map>

bjudger::Context ctx;
std::unordered_map<std::string, std::unique_ptr<bjudger::Problem>> &problems = ctx.problems;