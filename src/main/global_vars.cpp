#include <unordered_map>
#include <memory>
#include <string>
#include "problem.h"

std::unordered_map<std::string, std::unique_ptr<bjudger::Problem>> problems;