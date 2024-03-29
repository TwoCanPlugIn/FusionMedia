# ---------------------------------------------------------------------------
# Author:      Steven Adler based on (jongough aka. Jon Gough) Copyright: License:     wxWidgets License
# ---------------------------------------------------------------------------

## ----- When changing this file do NOT change the order in which sections occur        ----- ##
## ----- Changes should only be made between the section blocks that identify where     ----- ##
## ----- these changes should be. The whole configuration process relies on this        ----- ##
## ----- sequence to be successful                                                      ----- ##
##

##
## ----- Modify section below to include all the details for your plugin ----- ##
##

set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.1.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)


# define plugin name, owner and versions
set(VERBOSE_NAME "Fusion Media Player")
set(COMMON_NAME "Fusion Media Player")
set(TITLE_NAME "FUSIONMEDIAPLAYER")
set(PACKAGE_CONTACT "twocanplugin@hotmail.com")
set(PACKAGE "fusion_media_player")
set(SHORT_DESCRIPTION "Fusion Media Player, a remote control for Fusion media players")
set(LONG_DESCRIPTION "Fusion Media Player, a remote control that uses the TwoCan plugin to communicate over NMEA 2000 networks with Fusion media players") 
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_TWEAK "0")
set(VERSION_DATE "10/10/2021")
set(OCPN_MIN_VERSION "ov50")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "17")
set(TP_COMMENT "Release for O5 using CI")

set(PARENT "opencpn")

# The next line allows for setting up your own local webserver with git on it for testing purposes, the default is github.com
set(GIT_REPOSITORY_SERVER "github.com")

# Specifies cloudsmith user and repository names (note suffixes for repositories production, beta & alpha)
set(PROD "prod") # Standard Repos
set(BETA "beta") # Standard Repos
set(ALPHA "alpha") # Standard Repos

set(CLOUDSMITH_USER "steven-adler")
# set this if your cloudsmith repository name does not match your git repository name 
set(CLOUDSMITH_BASE_REPOSITORY "fusionmedia")

set(XML_INFO_URL "")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

## Uses SVG graphics for the toolbar icons
option(PLUGIN_USE_SVG "Use SVG graphics" ON)

set(CMAKE_CXX_STANDARD 17)

# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
# Don't use local version of GLU library
set(USE_LOCAL_GLU FALSE)
option(USE_GL "Enable OpenGL support" OFF)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

## Define the build type
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE
        "Release"
        CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

##
## ----- The statements below are used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

project(${PACKAGE})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")

include("PluginSetup")

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

set(CMAKE_VERBOSE_MAKEFILE ON)

option(Plugin_CXX11 "Use c++11" OFF)


##
## ----- do not change next section - needed to configure build process ----- ##
##

include("PluginConfigure")

##
## ----- Change below to match project requirements for source, headers, etc. ----- ##
##

## Fusion Media Player does not understand/use this definition
##add_definitions(-DUSE_S57)

## Fusion Media Player uses SVG for toolbar icons
add_definitions(-DPLUGIN_USE_SVG)

## JSON used for plugin messaging
if(APPLE)
    option(PI_JSON_SCHEMA_VALIDATOR "Use JSON Schema validator" OFF)
else(APPLE)
    option(PI_JSON_SCHEMA_VALIDATOR "Use JSON Schema validator" ON)
endif(APPLE)

## Need following for ocpn_plugin.h
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/libs/ocpn-api)
## Need following for wxJSON
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/wxJSON/include)
## And the project include dirs
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc)

SET(SOURCES src/mediaplayer_icon.cpp
            src/mediaplayer_plugin.cpp
            src/mediaplayer_dialog.cpp
            src/mediaplayer_dialogbase.cpp)


SET(JSON
    libs/wxJSON/src/jsonval.cpp
    libs/wxJSON/src/jsonreader.cpp
    libs/wxJSON/src/jsonwriter.cpp
)

TARGET_SOURCES(${PACKAGE_NAME} PUBLIC ${SOURCES} ${JSON})

##
## ----- do not change next section - needed to configure build process ----- ##
##

include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")

