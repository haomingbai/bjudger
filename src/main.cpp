#include <iostream>

#include "simple_c_compiler.h"

int main(int, char**){
    bjudger::SimpleCCompiler compiler("output", "/bin/gcc", "/bin/bsdbx");
    std::string src = R"(
#include <stdio.h>
int main(){
    printf("Hello, World!");
    return 0;
}
)";
    auto log = compiler.compile(src);
    std::cout << "Path: " << log.path << std::endl;
    std::cout << "Log: " << log.log << std::endl;
    return 0;
}
