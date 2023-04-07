cmake_minimum_required(VERSION 3.23)
project(EGEsnake)

set(CMAKE_CXX_STANDARD 20)
set(EGE_INC_DIR D:/BaiduNetdiskDownload/Programme/EGE20.08/include)
set(EGE_LINK_DIR D:/BaiduNetdiskDownload/Programme/EGE20.08/lib/mingw64)

set(CMAKE_CXX_FLAGS -mwindows)

include_directories(${EGE_INC_DIR})
link_directories(${EGE_LINK_DIR})

add_executable(EGEsnake main.cpp head.h others.cpp others.h play.cpp play.h structs.cpp structs.h paint.cpp paint.h)

target_link_libraries(EGEsnake libgraphics64.a
        libgdi32.a libimm32.a libmsimg32.a libole32.a
        liboleaut32.a libwinmm.a libuuid.a libgdiplus.a)