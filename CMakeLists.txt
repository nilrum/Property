cmake_minimum_required(VERSION 3.12)

project(Property)

set(CMAKE_CXX_STANDARD 17)

if(NOT EXISTS ${PUGI_DIR})
    exec_program("git" "" ARGS "clone --depth=1 https://github.com/zeux/pugixml ${CMAKE_CURRENT_SOURCE_DIR}/pugixml")
    message("Pugixml cloned")
    set(PUGI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pugixml CACHE FILEPATH "PUGI_DIR")
    message("Added PUGI_DIR = [${PUGI_DIR}]")
endif()

if(NOT EXISTS ${SIGSLOT_DIR})
    exec_program("git" "" ARGS "clone --depth=1 https://github.com/palacaze/sigslot ${CMAKE_CURRENT_SOURCE_DIR}/sigslot")
    message("Sigslot cloned")
    set(SIGSLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sigslot CACHE FILEPATH "SIGSLOT_DIR")
    message("Added SIGSLOT_DIR = [${SIGSLOT_DIR}]")
endif()

set(PROPERTY_HDR Types.h Variable.h PropertyClass.h PropertyClass.hpp Serialization.h PropertyEditor.h EnumInfo.h GlobalCustom.h)
set(PROPERTY_SRC Variable.cpp PropertyClass.cpp Serialization.cpp PropertyEditor.cpp ${PUGI_DIR}/src/pugixml.cpp EnumInfo.cpp GlobalCustom.cpp Types.cpp)

if(NOT TARGET Property)
    #add_definitions("-DMAGIC_ENUM_NO_CHECK_SUPPORT")
    add_library(Property ${PROPERTY_HDR} ${PROPERTY_SRC})
    target_include_directories(Property PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${PUGI_DIR}/src PUBLIC ${SIGSLOT_DIR}/include)
endif()

if(NOT TARGET gtest)
    if(NOT EXISTS ${GOOGLETEST_DIR})
        exec_program("git" "" ARGS "clone --depth=1 https://github.com/google/googletest ${CMAKE_CURRENT_SOURCE_DIR}/googletest")
        message("Googletest cloned")
        set(GOOGLETEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest CACHE FILEPATH "GOOGLETEST_DIR")
        message("Added GOOGLETEST_DIR = [${GOOGLETEST_DIR}]")
    endif()
    add_subdirectory(googletest)
endif()

add_subdirectory(tests)