# bjudger

`bjudger` is a judging server designed to function as an Online Judge (OJ) server. It provides a list of available problems and evaluates submitted solutions. This project aims to facilitate competitive programming by automating the judging process and offering a seamless experience for users to test their code against predefined problem sets.

## General Usage

### Installation

The bjudger, as a part of another OJ program, can be easily installed with CMake, which is an robust and easy to use building system. Currently, without cpack support, you can only build this software from source code from github. If you are building it on Arch Linux, it will be a piece of cake. First, we should install boost and build workflow fro source since there are no package `workflow` on both official repository and `AUR` repository,:

```bash
# Install boost and the build toolchains,
sudo pacman -S base-devel boost

# Clone the source code of workflow.
git clone https://github.com/sogou/workflow.git

# Build the workflow from source.
cd workflow
mkdir build
cd build
cmake .. -G Unix\ Makefiles
make -j$(grep 'processor' /proc/cpuinfo |  wc -l)
sudo make install
```

Then, we can clone this repository and build

```bash
# Get the source code.
cd
git clone https://github.com/haomingbai/bjudger.git

# Build and install.
cd bjudger && mkdir build cd build
cmake .. -G Unix\ Makefiles
make -j$(grep 'processor' /proc/cpuinfo |  wc -l)
make install
```

However, when trying the similar process on Ubuntu, something unexpected happened. Since Boost::process and maybe some other components have not been compiled in the official sources and though packed, cmake package is unsupported by the workflow in official sources, we should also compile and install boost.

```bash
sudo apt-get install build-essential cmake
git clone https://github.com/boostorg/boost.git --recurse-submodules
cd boost
./bootstrap.sh --prefix=/usr/local
./b2 -j$(grep 'processor' /proc/cpuinfo |  wc -l)
sudo ./b2 install
```

Then we can test the judger with the following command

```bash
bjudger
```

Automatically, the bjudger can automatically read the configuration file from `/etc/bjudger/config.json`.

### About configuration file

Let's first have a look at the configuration file:

```json
{
    "thread": 4,
    "port": 8080,
    "apis": [
        {
            "name": "/exist",
            "path": "/usr/local/lib/libbjudger_api_exist.so"
        },
        {
            "name": "/judge",
            "path": "/usr/local/lib/libbjudger_api_judge.so"
        }
    ],
    "problems": [
        {
            "path": "/usr/local/share/bjudger/example/problemA",
            "specialJudge": false,
            "judgers": [
                {
                    "name": "cpp",
                    "path": "/usr/local/lib/libbjudger_simple_cpp_judger.so",
                    "workingDirectory": "/tmp/bjudger/problemA/cpp",
                    "compilerPath": "/bin/g++",
                    "runnerPath": "",
                    "bsdbxPath": "/usr/local/bin/bsdbx",
                    "specialJudgerPath": "",
                    "compilerNum": 8,
                    "memoryLimit": 1024,
                    "timeLimit": 1000
                }
            ]
        }
    ]
}
```

As we can see, the first item of this file is "thread", which means the number of threads to be used by the server, while the second line, port, indicates that the judger will listen the 8080 port. Besides, by filling the "cert" and "key", you may have a https server.

The "apis" item is a list indicating the paths of the api functions. For each of the api, there should be two variables, namely "name" which shows the route path of the api and "path" indicating the path of the api function. The api function will be mentioned later.

The "problems" array is the most crucial part of this configuration. Since the ID of the problem is calculated by its content, it is only necessary for us to write down the "path" of the problem for the program to find it. "specialJudge" item is a boolean, showing whether this problem needs special judge. **The special judging function is currently unfinished!** The judger list is very necessary for all the problems for language support. In this list, you should fulfill the path of compiler (leave blank if unnecessary), the path of runner (same as compiler), the path of **special judger** (same as runner), the path of bsdbx(a sandbox), the memory limit, the supported language, time limit and the number of compilers.

## Plugins

The most inspiring and attractive function of this judger is plugin, which is all personalized. Currently, there are two types of plugins, the first one is called "api plugin" while another type is called "language" plugin. The api plugin should provide a function with C interface:

```C++
extern "C" 
{
    void api(bjudger::Context *, WFHttpTask *);
}
void api(bjudger::Context *context, WFHttpTask *task)
{
    /* ...Your Code... */
}
```

In the api plugin, the programmer should include the bjudger/context.h and then use the content provided by the context. If you want to affect the content of the context, you may use a reference or a pointer points to the required element. The WFHttpTask is a class provided by <workflow/HttpTask.h> and you can use it to get the request and response. The response should be set by the programmer of the plugin.

Another kind of plugin is called language plugin. For this kind of plugin, the programmer should provide such a kind of function with C interface and a class inherited from the `bjudger::Judger` class:

```C++
class MyJudger : public bjudger::Judger
{
    /* ...Your Code... */
    /* Overwrite the virtual functions */
};

extern "C" 
{
    void createJudger(char *, char *, char *, char *, int, size_t, size_t, char *);
}

bjudger::Judger* createJudger(char *workingDirectory, char *compilerPath, char *runnerPath, char *specialJudgerPath, int compilerNum, size_t memoryLimit, size_t timeLimit, char *bsdbxPath)
{
    return new MyJudger(/* ... */);
}
```

### About problem

The problem is a directory containing a markdown file and some other files depending on the type of the problem.

For ordinary problems, the directory should contain a markdown file named `content.md` and a a set of input and output files. The input files should be named as `a.in`, `b.in`, `c.in` and so on, while the output files should be named as `a.out`, `b.out`, `c.out` and so on. The directory's structure should be like this:

```bash
problemA
├── a.in
├── a.out
├── b.in
├── b.out
├── c.in
├── c.out
└── content.md
```

For special judge problems, the directory should contain a markdown file named `content.md`, a special judge file named `judge.cpp`. The directory's structure should be like this:

```bash
problemB
├── content.md
└── judge.cpp
```

When calculating the ID of the problem, the program will first create a string named original. Then, the hash of each file will be appended to the original string as the following order:

+ content.md
+ judge.cpp (if it is a special judge problem)
+ a.in (if it is a ordinary problem)
+ b.in
+ c.in
+ a.out
+ b.out
+ c.out

Then, the SHA512 hash of the original string will be calculated and the id of the problem will be the hash of the original string.

## Notes

The maintainer of this project has got illed and the project is currently unfinished. If you have any questions, please contact me at [haomingbai@hotmail.com](mailto:haomingbai@hotmail.com).
