# bjudger
`bjudger` is a judging server designed to function as an Online Judge (OJ) server. It provides a list of available problems and evaluates submitted solutions. This project aims to facilitate competitive programming by automating the judging process and offering a seamless experience for users to test their code against predefined problem sets.

## Developing Plan
The current version intended to support only standard io judge. The interface support of special judge will be left as a possible extension of this project. Currently, the constructor of the Problem class will accept two params, one of which is the problem's root directory and another is the flag to determine whether to initialize the Problem as a special judge. Unfortunately, now, we can only support the non-special ver. \
The ordinary judger will support the file of the .in as input and the .out with the same file name without extension as the expected output thus:
```
problem
├── 1.in
├── 1.out
├── 2.in
├── 2.out
└── content.md
```
In the future, the special judge will, maybe taken into consideration. At that time, the structure of the dir will be as follows:
```
problem
├── content.md
└── judge.cpp
```
In the special judge, there will be such code:
```C++
extern "C" {
    int judge(char *command);
}
```
