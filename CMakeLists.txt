cmake_minimum_required(VERSION 3.9)
project(rccpFlow)
project(columnGeneration)

set(CMAKE_CXX_STANDARD 11)
set(Boost_USE_STATIC_LIBS ON )
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DIL_STD" )

include_directories("/opt/ibm/ILOG/CPLEX_Studio1271/cplex/include" "/opt/ibm/ILOG/CPLEX_Studio1271/concert/include")
link_directories("/opt/ibm/ILOG/CPLEX_Studio1271/cplex/lib/x86-64_linux/static_pic" "/opt/ibm/ILOG/CPLEX_Studio1271/concert/lib/x86-64_linux/static_pic")

set(commonFiles Common/Graph.h Common/Graph.cpp Common/ReadData.h Common/ReadData.cpp)
set(rccpFlowHeaders RccpFlow/MyCutCallback.h RccpFlow/MyLazyCallback.h RccpFlow/RccpFlow.h RccpFlow/Util.h)
set(rccpFlowSources RccpFlow/MyCutCallback.cpp RccpFlow/MyLazyCallback.cpp RccpFlow/RccpFlow.cpp RccpFlow/Util.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(rccpFlow RccpFlow/Main.cpp ${commonFiles} ${rccpFlowHeaders} ${rccpFlowSources})
add_executable(rccpRunner Runner/RccpRunner.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

target_link_libraries(rccpFlow ilocplex concert cplex m pthread)


