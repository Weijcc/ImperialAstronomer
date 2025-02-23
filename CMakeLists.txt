cmake_minimum_required(VERSION 3.25)
project(ImperialAstronomer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH "C:/Application/Qt/6.8.2/msvc2022_64")

set(CMAKE_AUTOMOC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/HeaderFiles")
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/InterfaceFiles")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

file(GLOB HeaderFiles "${CMAKE_SOURCE_DIR}/HeaderFiles/*.h")
file(GLOB SourceFiles "${CMAKE_SOURCE_DIR}/SourceFiles/*.cpp")
file(GLOB ResourceFiles "${CMAKE_SOURCE_DIR}/ResourceFiles/*.qrc")
file(GLOB InterfaceFiles "${CMAKE_SOURCE_DIR}/InterfaceFiles/*.ui")

set(ResourceFile "${CMAKE_SOURCE_DIR}/ResourceFiles/BasicFiles/Icon.rc")

qt_add_binary_resources(ResourceBinrary ${ResourceFiles})

add_definitions(-DDEMO)

qt_add_executable(${PROJECT_NAME} WIN32
    ${HeaderFiles}
    ${SourceFiles}
    ${InterfaceFiles}
    ${ResourceFile})

add_dependencies(${PROJECT_NAME} ResourceBinrary)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS " /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/HeaderFiles"
    "${CMAKE_PREFIX_PATH}/include/QtZlib"
    "${CMAKE_SOURCE_DIR}/ThirdPartyFiles"
    "${CMAKE_PREFIX_PATH}/../../Tools/OpenSSLv3/Win_x64/include")

target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(${PROJECT_NAME} PRIVATE /Ox /fp:fast /utf-8 /Gy /GF /GL /Qpar /arch:AVX2)
  target_link_options(${PROJECT_NAME} PRIVATE /LTCG /OPT:REF,ICF)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Gui Qt::Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${CMAKE_PREFIX_PATH}/../../Tools/OpenSSLv3/Win_x64/lib/libssl.lib"
    "${CMAKE_PREFIX_PATH}/../../Tools/OpenSSLv3/Win_x64/lib/libcrypto.lib")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    DEPENDS ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_PREFIX_PATH}/../../Tools/OpenSSLv3/Win_x64/bin/libssl-3-x64.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_PREFIX_PATH}/../../Tools/OpenSSLv3/Win_x64/bin/libcrypto-3-x64.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
