#!/usr/bin/env -S cmake -P

function(task_generate)
    set(CMAKE_MESSAGE_LOG_LEVEL DEBUG)

    # TODO: Use $ENV{TMP} and string(RANDOM) to use a temporary directory
    message(VERBOSE "Removing & recreating generate")
    file(REMOVE_RECURSE generate)
    file(MAKE_DIRECTORY generate)

    file(DOWNLOAD "https://nodejs.org/dist/index.json" generate/index.json)
    file(READ generate/index.json index_json)
    string(JSON node_version GET "${index_json}" .0.version)
    string(SUBSTRING "${node_version}" 1 -1 node_version)

    set(targets win-x64 win-arm64 darwin-x64 darwin-arm64 linux-x64 linux-arm64)
    message(DEBUG "targets=${targets}")
    
    foreach(target IN LISTS targets)
        message(DEBUG "target=${target}")

        if(target MATCHES "^win-")
            set(archive_ext ".zip")
        elseif(target MATCHES "^linux-")
            set(archive_ext ".tar.xz")
        else()
            set(archive_ext ".tar.gz")
        endif()
        message(DEBUG "archive_ext=${archive_ext}")

        set(url "https://nodejs.org/dist/v${node_version}/node-v${node_version}-${target}${archive_ext}")
        set(dest "generate/node-${target}${archive_ext}")
        message(STATUS "Downloading ${url} to ${dest}")
        file(DOWNLOAD "${url}" "${dest}")

        message(VERBOSE "Extracting ${dest} to generate")
        file(ARCHIVE_EXTRACT INPUT "${dest}" DESTINATION generate)

        message(VERBOSE "Removing ${dest}")
        file(REMOVE "${dest}")
        
        message(VERBOSE "Renaming generate/node-v${node_version}-${target} to generate/node-${target}")
        file(RENAME "generate/node-v${node_version}-${target}" "generate/node-${target}")
    endforeach()

    foreach(target IN LISTS targets)
        message(DEBUG "target=${target}")

        if(target MATCHES "^win-")
            message(VERBOSE "Creating generate/node-${target}-temp")
            file(MAKE_DIRECTORY "generate/node-${target}-temp")

            message(VERBOSE "Renaming generate/node-${target}/node.exe to generate/node-${target}-temp/node.exe")
            file(RENAME "generate/node-${target}/node.exe" "generate/node-${target}-temp/node.exe")
            
            if(EXISTS generate/node-win)
                message(VERBOSE "Removing generate/node-${target}")
                file(REMOVE_RECURSE "generate/node-${target}")
            else()
                if(EXISTS generate/node-node_modules)
                    message(VERBOSE "Removing generate/node-${target}/node_modules")
                    file(REMOVE_RECURSE "generate/node-${target}/node_modules")
                else()
                    message(VERBOSE "Moving generate/node-${target}/node_modules to generate/node-node_modules")
                    file(RENAME "generate/node-${target}/node_modules" "generate/node-node_modules")
                endif()

                message(VERBOSE "Renaming generate/node-${target} to generate/node-win")
                file(RENAME "generate/node-${target}" "generate/node-win")
            endif()
            
            message(VERBOSE "Renaming generate/node-${target}-temp to generate/node-${target}")
            file(RENAME "generate/node-${target}-temp" "generate/node-${target}")
        else()
            set(platform_specific "bin/node" "include/node/config.gypi")
            message(STATUS "Preserving platform-specific files ${platform_specific}")
            
            message(VERBOSE "Creating generate/node-${target}-temp")
            file(MAKE_DIRECTORY "generate/node-${target}-temp")
            
            foreach(src_relative IN LISTS platform_specific)
                set(dest "generate/node-${target}-temp/${src_relative}")
                cmake_path(GET dest PARENT_PATH dest_parent)
            
                message(VERBOSE "Creating directory ${dest_parent}")
                file(MAKE_DIRECTORY "${dest_parent}")
            
                message(VERBOSE "Renaming generate/node-${target}/${src_relative} to ${dest}")
                file(RENAME "generate/node-${target}/${src_relative}" "${dest}")
            endforeach()
            
            if(EXISTS generate/node-unixlike)
                message(VERBOSE "Removing generate/node-${target}")
                file(REMOVE_RECURSE "generate/node-${target}")
            else()
                if(EXISTS generate/node-node_modules)
                    message(VERBOSE "Removing generate/node-${target}/node_modules")
                    file(REMOVE_RECURSE "generate/node-${target}/node_modules")
                else()
                    message(VERBOSE "Moving generate/node-${target}/node_modules to generate/node-node_modules")
                    file(RENAME "generate/node-${target}/node_modules" "generate/node-node_modules")
                endif()

                message(VERBOSE "Renaming generate/node-${target} to generate/node-unixlike")
                file(RENAME "generate/node-${target}" "generate/node-unixlike")
            endif()
            
            message(VERBOSE "Renaming generate/node-${target}-temp to generate/node-${target}")
            file(RENAME "generate/node-${target}-temp" "generate/node-${target}")
        endif()
    endforeach()

    # https://gitlab.kitware.com/cmake/cmake/-/issues/21653
    # Can't use file(ARCHIVE_CREATE) with a different base directory
    # so instead we rely on the zip CLI. 🤷‍♀️

    find_program(zip_command zip REQUIRED)

    message(VERBOSE "Creating node-win.zip")
    execute_process(
        COMMAND "${zip_command}" -rq "${CMAKE_BINARY_DIR}/node-win.zip" .
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/generate/node-win"
        COMMAND_ECHO STDERR
        COMMAND_ERROR_IS_FATAL ANY
    )
    
    message(VERBOSE "Creating node-unixlike.zip")
    execute_process(
        COMMAND "${zip_command}" -rq "${CMAKE_BINARY_DIR}/node-unixlike.zip" .
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/generate/node-unixlike"
        COMMAND_ECHO STDERR
        COMMAND_ERROR_IS_FATAL ANY
    )
    
    message(VERBOSE "Creating node-node_modules.zip")
    execute_process(
        COMMAND "${zip_command}" -rq "${CMAKE_BINARY_DIR}/node-node_modules.zip" .
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/generate/node-node_modules"
        COMMAND_ECHO STDERR
        COMMAND_ERROR_IS_FATAL ANY
    )
    
    foreach(target IN LISTS targets)
        message(DEBUG "target=${target}")

        message(VERBOSE "Creating node-${target}.zip")
        execute_process(
            COMMAND "${zip_command}" -rq "${CMAKE_BINARY_DIR}/node-${target}.zip" .
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/generate/node-${target}"
            COMMAND_ECHO STDERR
            COMMAND_ERROR_IS_FATAL ANY
        )
    endforeach()
endfunction()

if(NOT TASK_NAME AND CMAKE_ARGV3)
    set(TASK_NAME "${CMAKE_ARGV3}")
else()
    message(FATAL_ERROR "no task")
endif()
if(TASK_NAME STREQUAL generate)
    task_generate()
else()
    message(FATAL_ERROR "no such task")
endif()
