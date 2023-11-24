cmake_minimum_required(VERSION 3.21)

project(FagotronBot LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

SET(CMAKE_USE_RELATIVE_PATHS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(CURL 7.58.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CODE_COVERAGE "-fprofile-arcs -ftest-coverage")
    set(LIBCODE_COVERAGE "-lgcov --coverage")
    set(OPTIM "-ggdb -Og")
    add_definitions(-DWAKA_DEBUG)
else()
    set(OPTIM "-Ofast")
    add_definitions(-DWAKA_RELEASE)
endif()

file(STRINGS "token.txt" TOKEN_CONTENT)
add_definitions(-DFAGOTRON_TOKEN="${TOKEN_CONTENT}")
set(BasicCorpusFile ${CMAKE_CURRENT_SOURCE_DIR}/corpus.yml)

set(CFLAGS "${CFLAGS} ${OPTIM}")
set(CXXFLAGS "${CXXFLAGS} ${OPTIM}")

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
set(TGBOT_DISABLE_NAGLES_ALGORITHM ON)
add_subdirectory(tgbot-cpp)
add_subdirectory(SQLiteCpp)
add_subdirectory(TinyLog)
set(YAML_BUILD_SHARED_LIBS OFF)
set(YAML_CPP_BUILD_TESTS=OFF)
set(YAML_CPP_BUILD_TOOLS=OFF)
set(YAML_CPP_BUILD_CONTRIB=OFF)
add_subdirectory(yaml-cpp)

set(CPPSRC
  fagotron.cc
  play.cc
  commands.cc
  db.cc
  send.cc
  corpus.cc
)

add_custom_target(BasicCorpus ALL DEPENDS ${BasicCorpusFile})
get_filename_component(BasicCorpusFileName ${BasicCorpusFile} NAME_WE)
add_custom_command(TARGET BasicCorpus       
        COMMENT "Copying settings file ${BasicCorpusFile}"
        COMMAND ${CMAKE_COMMAND} -E copy ${BasicCorpusFile} "${CMAKE_BINARY_DIR}/${BasicCorpusFileName}.yml"
)

add_executable(FagotronBot ${CPPSRC})
add_dependencies(FagotronBot BasicCorpus)
set_target_properties(FagotronBot PROPERTIES OUTPUT_NAME "fagotron")

target_include_directories(FagotronBot PUBLIC TinyLog yaml-cpp/include)

target_link_libraries(FagotronBot PUBLIC TgBot ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES} tinyLog SQLiteCpp yaml-cpp)