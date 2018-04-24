cmake_minimum_required(VERSION 3.9)
project(RccpFlow)

set(CMAKE_CXX_STANDARD 11)

set(headers Source/Graph.h Source/MyCutCallback.h Source/MyLazyCallback.h Source/RccpFlow.h Source/ReadData.h Source/Util.h)
set(source Source/Main.cpp Source/Graph.cpp Source/MyCutCallback.cpp Source/MyLazyCallback.cpp Source/RccpFlow.cpp Source/ReadData.cpp Source/Util.cpp)

set(Boost_USE_STATIC_LIBS ON )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DIL_STD" )

include_directories("/opt/ibm/ILOG/CPLEX_Studio1271/cplex/include" "/opt/ibm/ILOG/CPLEX_Studio1271/concert/include")
link_directories("/opt/ibm/ILOG/CPLEX_Studio1271/cplex/lib/x86-64_linux/static_pic" "/opt/ibm/ILOG/CPLEX_Studio1271/concert/lib/x86-64_linux/static_pic")

add_executable(RccpFlow ${headers} ${source})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

target_link_libraries(RccpFlow ilocplex concert cplex m pthread)