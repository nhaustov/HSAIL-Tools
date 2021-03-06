include_directories(${PROJECT_SOURCE_DIR}/libHSAIL)
include_directories(${PROJECT_BINARY_DIR}/libHSAIL/generated)

include_directories(${PROJECT_SOURCE_DIR}/libHSAIL-AMD)

include_directories(${LLVM_PATH}/include)

include_directories(${LLVM_INCLUDE_DIR})

if(UNIX)
  include_directories(${LLVM_PATH}/linux/include)
else()
  include_directories(${LLVM_PATH}/win32/include)
endif()

add_executable(HSAILasm HSAILAsm.cpp)
target_link_libraries(HSAILasm hsail hsail-amd)
add_dependencies(HSAILasm libhsail-includes)

# FIXME: We also need to get the link dependencies of the libLLVMSupport
target_link_libraries(HSAILasm ${LLVM_SUPPORT_LIB})

find_library(CURSES_LIB curses)

#include(LLVM-Config.cmake)

if(BUILD_LIBBRIGDWARF)
  include_directories(${LIBELF_INCLUDE_DIRS})

  target_link_libraries(HSAILasm ${LIBELF_LIBRARIES})
  target_link_libraries(HSAILasm ${LIBDWARF_LIBRARIES})
endif()

if(CURSES_LIB)
  # FIXME: Find dependencies pulled in by LLVMSupport
  target_link_libraries(HSAILasm ${CURSES_LIB})
endif()
if(UNIX)
  target_link_libraries(HSAILasm tinfo)
endif()

install(TARGETS HSAILasm
  RUNTIME DESTINATION bin COMPONENT libhsail
)

set(HSAILASM $<TARGET_FILE:HSAILasm>)
set(test "${PROJECT_SOURCE_DIR}/tests/hsail_tests_p.hsail")

add_test(NAME HSAILAsm-help
         COMMAND ${HSAILASM} -help
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME HSAILAsm-version
         COMMAND ${HSAILASM} -version
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME HSAILAsm-assemble
         COMMAND ${HSAILASM} -assemble ${test} -o test.brig
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME HSAILAsm-disassemble
         COMMAND ${HSAILASM} -disassemble test.brig
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME HSAILAsm-decode
         COMMAND ${HSAILASM} -decode test.brig -o test.yaml
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if(BUILD_LIBBRIGDWARF)
add_test(NAME HSAILAsm-assemble-g
         COMMAND ${HSAILASM} -assemble -g ${test} -o test-g.brig
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME HSAILAsm-disassemble-g
         COMMAND ${HSAILASM} -disassemble test-g.brig
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME HSAILAsm-assemble-g-odebug
         COMMAND ${HSAILASM} -assemble -g ${test} -odebug test-g.dbg -o test-g.brig
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME HSAILAsm-assemble-g-include-source
         COMMAND ${HSAILASM} -assemble -g -include-source ${test} -o test-g.brig
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

endif()
