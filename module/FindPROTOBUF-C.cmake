#------------------------------------------------------------------------------
#
#  Copyright (C) 2010  Artem Rodygin
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#------------------------------------------------------------------------------
#
#  This module finds if C API of PROTOBUF library is installed and determines where required
#  include files and libraries are. The module sets the following variables:
#
#    PROTOBUF_FOUND         - system has PROTOBUF
#    PROTOBUF_INCLUDE_DIR   - the PROTOBUF include directory
#    PROTOBUF_C_LIBRARIES     - the libraries needed to use PROTOBUF
#    PROTOBUF_DEFINITIONS   - the compiler definitions, required for building with PROTOBUF
#    PROTOBUF_VERSION_MAJOR - the major version of the PROTOBUF release
#    PROTOBUF_VERSION_MINOR - the minor version of the PROTOBUF release
#
#  You can help the module to find PROTOBUF by specifying its root path
#  in environment variable named "PROTOBUF_ROOTDIR". If this variable is not set
#  then module will search for files in "/usr/local" and "/usr" by default.
#
#------------------------------------------------------------------------------

# TIP , usage of protobuf
# http://stackoverflow.com/questions/19343018/cmake-and-findprotobuf

SET(PROTOBUF_FOUND TRUE)

# SEARCH FOR HEADER

FIND_PATH(PROTOBUF_INCLUDE_DIR
        NAMES "google/protobuf-c/protobuf-c.h"
        PATHS "/usr/local"
        "/usr"
        ENV PROTOBUF_ROOTDIR
        PATH_SUFFIXES "include")

# HEADER IS FOUND

IF (PROTOBUF_INCLUDE_DIR)

    SET(PROTOBUF_INCLUDE_DIR "${PROTOBUF_INCLUDE_DIR}/google/protobuf-c")

    # SEARCH FOR LIBRARY

    FIND_LIBRARY(PROTOBUF_C_LIBRARIES
            NAMES "libprotobuf-c.so"
            PATHS "/usr/local"
            "/usr"
            ENV PROTOBUF_ROOTDIR
            PATH_SUFFIXES "lib")

ENDIF (PROTOBUF_INCLUDE_DIR)

# HEADER IS NOT FOUND

IF (NOT PROTOBUF_INCLUDE_DIR)
    SET(PROTOBUF_FOUND FALSE)
ENDIF (NOT PROTOBUF_INCLUDE_DIR)

# LIBRARY IS NOT FOUND

IF (NOT PROTOBUF_C_LIBRARIES)
    SET(PROTOBUF_FOUND FALSE)
ENDIF (NOT PROTOBUF_C_LIBRARIES)

SET(PROTOBUF_ERROR_MESSAGE "Unable to find PROTOBUF library")

IF (NOT PROTOBUF_FOUND)
    SET(PROTOBUF_ERROR_MESSAGE "Unable to find PROTOBUF library v${PROTOBUF_FIND_VERSION} (${PROTOBUF_FOUND_VERSION} was found)")
ENDIF (NOT PROTOBUF_FOUND)

# ADD DEFINITIONS

IF (PROTOBUF_FOUND)

    IF (CMAKE_SYSTEM_PROCESSOR MATCHES ia64)
        SET(PROTOBUF_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
    ELSEIF (CMAKE_SYSTEM_PROCESSOR MATCHES amd64)
        SET(PROTOBUF_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
    ELSEIF (CMAKE_SYSTEM_PROCESSOR MATCHES x86_64)
        SET(PROTOBUF_DEFINITIONS "-D_REENTRANT -D_FILE_OFFSET_BITS=64")
    ELSE (CMAKE_SYSTEM_PROCESSOR MATCHES ia64)
        SET(PROTOBUF_DEFINITIONS "-D_REENTRANT")
    ENDIF (CMAKE_SYSTEM_PROCESSOR MATCHES ia64)

ENDIF (PROTOBUF_FOUND)

# FINAL STATUS MESSAGES

IF (PROTOBUF_FOUND)

    IF (NOT PROTOBUF_FIND_QUIETLY)
        MESSAGE(STATUS "PROTOBUF-C")
    ENDIF (NOT PROTOBUF_FIND_QUIETLY)

    MARK_AS_ADVANCED(PROTOBUF_INCLUDE_DIR
            PROTOBUF_C_LIBRARIES
            PROTOBUF_DEFINITIONS)

ELSE (PROTOBUF_FOUND)

    IF (PROTOBUF_FIND_REQUIRED)
        MESSAGE(SEND_ERROR "${PROTOBUF_ERROR_MESSAGE}")
    ENDIF (PROTOBUF_FIND_REQUIRED)

ENDIF (PROTOBUF_FOUND)

