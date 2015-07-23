cmake_minimum_required(VERSION 3.2)
project (textures)

find_program(TC_COMMAND tc)

function(compress_to_ktx filename outputfiles)
    get_filename_component(base ${filename} NAME_WE)
    set(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})
    set(outfile ${base_abs}.ktx)

    add_custom_command(OUTPUT ${outfile} 
                       COMMAND ${TC_COMMAND}
                           -d ${outfile}
                           -t 4 ${filename}
                       DEPENDS "${filename}")
    set(${outputfiles} ${${outputfiles}} ${outfile} PARENT_SCOPE)
endfunction()

file(GLOB texture_files *.png)
foreach(file ${texture_files})
    compress_to_ktx(${file} OUTPUTFILES)
endforeach()

add_custom_target(textures DEPENDS ${OUTPUTFILES})
