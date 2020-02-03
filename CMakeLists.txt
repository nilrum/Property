cmake_minimum_required(VERSION 3.14)

project(Property)

set(CMAKE_CXX_STANDARD 17)

include_directories(pugixml/src)

set(PROPERTY_HDR Types.h Variable.h PropertyClass.h PropertyClass.hpp Serialization.h PropertyEditor.h)
set(PROPERTY_SRC Variable.cpp PropertyClass.cpp Serialization.cpp PropertyEditor.cpp pugixml/src/pugixml.cpp)

add_library(Property ${PROPERTY_HDR} ${PROPERTY_SRC})
message("[${CMAKE_SOURCE_DIR}]")

#Если gtest не подключен
if(NOT EXISTS ${gtest_SOURCE_DIR})
    if( NOT EXISTS ./googletest)
        exec_program("git" "" ARGS "clone --depth=1 https://github.com/google/googletest ../googletest")
        message("Googletest cloned")
    endif()
    add_subdirectory(googletest)
endif()

if( NOT EXISTS ./pugixml)
    exec_program("git" "" ARGS "clone --depth=1 https://github.com/zeux/pugixml ../pugixml")
    message("Pugixml cloned")
endif()

add_subdirectory(tests)