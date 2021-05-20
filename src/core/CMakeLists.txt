cmake_minimum_required(VERSION 3.8)

project(gnome_core VERSION 0.1)

set(Boost_USE_STATIC_LIBS     ON)
if(CMAKE_BUILD_TYPE MATCHES Debug)
set(Boost_USE_DEBUG_LIBS     ON)
else()
set(Boost_USE_RELEASE_LIBS    ON)
endif()
set(Boost_USE_MULTITHREADED   ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(
    Boost 1.76.0 
    COMPONENTS 
    date_time 
    chrono 
    container
    json
    program_options
)

set(OPENSSL_USE_STATIC_LIBS ON)

find_package(OpenSSL 1.1 REQUIRED)

message("BOOST LIBRARIES LOCATION: " ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

if(Boost_FOUND AND OPENSSL_FOUND)

set(
    SOURCES
    gnome.h
    gnome.cpp
    block.cpp
    block.h
    chain.cpp
    chain.h
    crypto.h
    currency.h
    miner.cpp
    miner.h
    pool.cpp
    pool.h
    random.h
    transaction.cpp
    transaction.h
    utils.h
    wallet.h
    wallet.cpp
)

if(WIN32)
    add_compile_options(/D__USE_W32_SOCKETS)
ELSE()

ENDIF()

add_library(
    ${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

set(
    ${PROJECT_NAME}_INCLUDE_DIRS 
    ${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
)

ENDIF()