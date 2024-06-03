cmake_minimum_required(VERSION 3.10)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include(ExternalProject)

set(source_dir "${CMAKE_BINARY_DIR}/libserial-src")
set(build_dir "${CMAKE_BINARY_DIR}/libserial-build")

EXTERNALPROJECT_ADD(
  libserial
  GIT_REPOSITORY    https://github.com/lukaskaz/lib-serial.git
  GIT_TAG           main
  PATCH_COMMAND     ${patching_cmd}
  PREFIX            libserial-workspace
  SOURCE_DIR        ${source_dir}
  BINARY_DIR        ${build_dir}
  CONFIGURE_COMMAND mkdir /${build_dir}/build &> /dev/null
  BUILD_COMMAND     cd ${build_dir}/build && cmake -D BUILD_SHARED_LIBS=ON
                    ${source_dir} && make -j 4
  UPDATE_COMMAND    ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

include_directories(${source_dir}/inc)
link_directories(${build_dir}/build)

set(source_dir "${CMAKE_BINARY_DIR}/libclimenu-src")
set(build_dir "${CMAKE_BINARY_DIR}/libclimenu-build")

EXTERNALPROJECT_ADD(
  libclimenu
  GIT_REPOSITORY    https://github.com/lukaskaz/lib-climenu.git
  GIT_TAG           main
  PATCH_COMMAND     ${patching_cmd}
  PREFIX            libclimenu-workspace
  SOURCE_DIR        ${source_dir}
  BINARY_DIR        ${build_dir}
  CONFIGURE_COMMAND mkdir /${build_dir}/build &> /dev/null
  BUILD_COMMAND     cd ${build_dir}/build && cmake -D BUILD_SHARED_LIBS=ON
                    ${source_dir} && make -j 4
  UPDATE_COMMAND    ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

include_directories(${source_dir}/inc)
link_directories(${build_dir}/build)

set(source_dir "${CMAKE_BINARY_DIR}/libnlohmann-src")
set(build_dir "${CMAKE_BINARY_DIR}/libnlohmann-build")

EXTERNALPROJECT_ADD(
  libnlohmann
  GIT_REPOSITORY    https://github.com/nlohmann/json.git
  GIT_TAG           v3.11.3
  PATCH_COMMAND     ${patching_cmd}
  PREFIX            libnlohmann-workspace
  SOURCE_DIR        ${source_dir}
  BINARY_DIR        ${build_dir}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  UPDATE_COMMAND    ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

include_directories(${source_dir}/include)
link_directories(${build_dir}/build)
