#include <iostream>

#include "lang/cpp/simple_cpp_compiler.h"
#include "lang/complex_compiler.h"

int main(int, char**){
    bjudger::ComplexCompiler compiler;
    std::unique_ptr<bjudger::Compiler> cCompiler = std::make_unique<bjudger::SimpleCppCompiler>("output", "/usr/bin/gcc", "/usr/bin/bsdbx");
    std::string src = R"(
#include <stdio.h>
int main(){
    printf("Hello, World!");
    return 0;
}
)";
    compiler.addCompiler(std::move(cCompiler));
    auto log = compiler.compile(src);
    std::cout << "Path: " << log.path << std::endl;
    std::cout << "Log: " << log.log << std::endl;
    return 0;
}
