# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# project name and language
project(thames LANGUAGES C)

add_executable(thames
    cmdline.c
    errcheck.c
    extensions.c
    isis.c
    isisfile.c
    isisload.c
    thames.c
    utils.c
    z80.c
    cbops.h
    cmdline.h
    config.h
    edops.h
    errcheck.h
    extensions.h
    isis.h
    isisfile.h
    isisload.h
    thames.h
    z80.h
    z80ops.h
    )

target_compile_options(thames
    PRIVATE
    "-D_CRT_SECURE_NO_WARNINGS"
    )

include(CheckIncludeFiles)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(fcntl.h HAVE_FNCTL_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(errno.h HAVE_ERRNO_H)
CHECK_INCLUDE_FILES(limits.h HAVE_LIMITS_H)
CHECK_INCLUDE_FILES(conio.h HAVE_CONIO_H)
CHECK_INCLUDE_FILES(io.h HAVE_IO_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file(config.in config.h @ONLY)


