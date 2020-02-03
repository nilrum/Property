project(PropertyTest)

#Add include files
foreach(F ${PROPERTY_HDR})
    set(INCL_PROPERTY_HDR ${INCL_PROPERTY_HDR} ../${F})
endforeach()

#Add test files
set(PROPERTY_TESTS VariableTest.cpp PropertyTest.cpp TestCasePropertyClass.h TestCasePropertyClass.cpp PropertyEditorTest.cpp)

#Target
add_executable(PropertyTest ${PROPERTY_TESTS} ${INCL_PROPERTY_HDR})
target_link_libraries(PropertyTest PUBLIC Property gtest gtest_main gmock)

#Add include dirs
include_directories(.. ${gtest_SOURCE_DIR} ${gtest_SOURCE_DIR}/include)

