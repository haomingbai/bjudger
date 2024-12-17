#include <iostream>

#include "lang/cpp/cpp_judger.h"

int main(int, char**){
    std::string src = R"(
#include <stdio.h>
int main(){
    printf("Hello, World!");
    return 0;
}
)";
bjudger::CppJudger judger("working_directory", "/bin/gcc", "/bin/bsdbx", 10);
}
