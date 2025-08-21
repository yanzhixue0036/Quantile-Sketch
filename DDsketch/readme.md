构建、编译和运行
现在就可以构建和运行我们的项目了，就是先运行 cmake 命令来构建项目，然后使用你选择的编译工具进行编译。

先从命令行进入到 step1 目录，并创建一个构建目录 build，接下来，进入 build 目录并运行 CMake 来配置项目，并生成构建系统：

mkdir build
cd build
cmake -G"MinGW Makefiles" ..
构建系统是需要指定 CMakeLists.txt 所在路径，此时在 build 目录下，所以用 .. 表示 CMakeLists.txt 在上一级目录。

Windows 下，CMake 默认使用微软的 MSVC 作为编译器，我想使用 MinGW 编译器，可以通过 -G 参数来进行指定，只有第一次构建项目时需要指定。

此时在 build 目录下会生成 Makefile 文件，然后调用编译器来实际编译和链接项目：

cmake --build .
--build 指定编译生成的文件存放目录，其中就包括可执行文件，. 表示存放到当前目录，

在 build 目录下生成了一个 Tutorial.exe 可执行文件，试着执行它：

> Tutorial.exe 5
The square root of 5 is 2.23607
该程序计算 5 的平方根，从输出结果看已经得到了正确的结果。

此时目录结构为：

step1/
    build/
    CMakeLists.txt
    tutorial.cpp
外部构建与内部构建
这里创建了一个 build 目录存放编译产物，可以避免编译产物与代码文件混在一起，这种叫做外部构建。

还有一种内部构建，即直接在项目根目录下进行构建系统与编译，这时构建和编译命令就更改为：

cmake -G"MinGW Makefiles" .
cmake --build .
内部构建会使得项目文件很混乱，一般直接用外部构建即可。