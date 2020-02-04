cmake_minimum_required(VERSION 3.14)

project(Property)

set(CMAKE_CXX_STANDARD 17)

set(PROPERTY_HDR Types.h Variable.h PropertyClass.h PropertyClass.hpp Serialization.h PropertyEditor.h)
set(PROPERTY_SRC Variable.cpp PropertyClass.cpp Serialization.cpp PropertyEditor.cpp pugixml/src/pugixml.cpp)

if(NOT TARGET Property)
    add_library(Property ${PROPERTY_HDR} ${PROPERTY_SRC})
    target_include_directories(Property PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE pugixml/src)
endif()

#Если gtest не подключен
if(NOT EXISTS ${gtest_SOURCE_DIR})
    if( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
        exec_program("git" "" ARGS "clone --depth=1 https://github.com/google/googletest ${CMAKE_CURRENT_SOURCE_DIR}/googletest")
        message("Googletest cloned")
    endif()
    add_subdirectory(googletest)
endif()

if( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pugixml)
    exec_program("git" "" ARGS "clone --depth=1 https://github.com/zeux/pugixml ${CMAKE_CURRENT_SOURCE_DIR}/pugixml")
    message("Pugixml cloned")
endif()

add_subdirectory(tests)