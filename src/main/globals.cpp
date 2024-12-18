#include <map>
#include <memory>
#include <string>
#include "problem.h"

std::map<std::string, std::unique_ptr<bjudger::Problem>> problems;