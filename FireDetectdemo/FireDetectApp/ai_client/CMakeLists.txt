#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 3.2) 
set(CMAKE_VERBOSE_MAKEFILE on)
#2.project name，指定项目的名称，一般和项目的文件夹名称对应 
PROJECT(mpreivew_app)

set(LIB_NAME "ai_client") 
SET(MAIN_SRC ${CMAKE_SOURCE_DIR}/ai_client/)


set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(LIBRARY_OUTPUT_PATH ${MAIN_SRC}/../lib/)


set(SRC_DIR "${MAIN_SRC}/src/")
set(INC_DIR "${MAIN_SRC}/include/")
set(INC_RES_DIR "${MAIN_SRC}/../")
set(EXT_INC_DIR "${MAIN_SRC}/extend/include/")

set(EXT_LIB_DIRS "${MAIN_SRC}/extend/lib/")
set(CURL "curl;") 
#3.head file path，头文件目录
INCLUDE_DIRECTORIES(
	${EXT_INC_DIR}/
	${INC_RES_DIR}/
	${INC_DIR}
	)

LINK_DIRECTORIES( ${EXT_LIG_DIRS})

aux_source_directory(${SRC_DIR} DIR_SRCS)

#5.set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(TEST_MATH ${DIR_SRCS}
	)

file(GLOB my_src ${SRC_DIR}/*.c  ${SRC_DIR}/*.cpp)

#6.add executable file添加要编译的可执行文件
#add_executable(ai_client ${my_src})

#add_library(${LIB_NAME} SHARED ${DIR_SRCS} )
add_library(${LIB_NAME} SHARED ${DIR_SRCS} )

#7.add link library，添加可执行文件所需要的库，比如我们用到了libm.so（命名规则：lib+name+.so），就添加该库的名称
target_link_libraries(ai_client ${CURL} "-lm -lpthread")

set(CMAKE_CXX_FLAGS "-std=c++0x  -lm -lpthread")
set(CMAKE_C_FLAGS " -lm -lpthread")

