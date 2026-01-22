function(FlatGenerateFun)
  # Parse function arguments (保持不变)
  set(options)
  set(one_value_args "TARGET" "INCLUDE_PREFIX" "BINARY_SCHEMAS_DIR" "OUTPUT_DIR")
  set(multi_value_args "SCHEMAS" "INCLUDE" "FLAGS")
  cmake_parse_arguments(PARSE_ARGV 0 FLATBUFFERS_GENERATE_HEADERS "${options}" "${one_value_args}" "${multi_value_args}")

  # 1. 确保找到 flatc 可执行文件（这是你之前运行失败的核心原因）
  set(FLATC_DEPENDS "")
  if(TARGET flatbuffers::flatc)
    set(FLATC flatbuffers::flatc)
    set(FLATC_DEPENDS flatbuffers::flatc)
  elseif(FLATBUFFERS_FLATC_EXECUTABLE)
    set(FLATC "${FLATBUFFERS_FLATC_EXECUTABLE}")
  else()
    find_program(FLATC_BIN flatc)
    if(FLATC_BIN)
      set(FLATC "${FLATC_BIN}")
    else()
    set(FLATC "${FLATC_BIN}")
      message(FATAL_ERROR "无法定位 flatc 可执行文件！请确保安装并设置环境变量，或手动指定 FLATBUFFERS_FLATC_EXECUTABLE。")
    endif()
  endif()

  # 2. 确定所有 Schema 文件的**公共基准路径** (Base Directory)
  # 这是实现“相对路径输出”的关键
  get_filename_component(first_schema_dir "${FLATBUFFERS_GENERATE_HEADERS_SCHEMAS}" DIRECTORY)
  set(schemas_base_dir "${first_schema_dir}")
  # 如果需要更精确的公共前缀计算，可以使用其他方法，这里假设第一个文件的目录就是基准目录

  # 3. 设置输出根目录
  # 如果用户未指定 OUTPUT_DIR，则默认使用当前源码目录作为输出根目录
  if ("${FLATBUFFERS_GENERATE_HEADERS_OUTPUT_DIR}" STREQUAL "")
      set(FLATBUFFERS_GENERATE_HEADERS_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
  set(generated_target_root_dir "${FLATBUFFERS_GENERATE_HEADERS_OUTPUT_DIR}")


  set(include_params "")
  foreach (include_dir ${FLATBUFFERS_GENERATE_HEADERS_INCLUDE})
    list(APPEND include_params -I "${include_dir}")
  endforeach()

  set(generated_custom_commands "")
  set(all_generated_header_files "")
  set(all_generated_source_files "")
  set(all_generated_binary_files "")

  # 4. 遍历 Schema，计算并生成文件
  foreach(schema ${FLATBUFFERS_GENERATE_HEADERS_SCHEMAS})
    get_filename_component(filename ${schema} NAME_WE)
    get_filename_component(schema_dir ${schema} DIRECTORY)
    
    # 计算当前 schema 目录相对于基准目录的路径
    file(RELATIVE_PATH schema_rel_dir "${schemas_base_dir}" "${schema_dir}")

    # 确定生成的文件的完整输出目录
    set(current_generated_output_dir "${generated_target_root_dir}/${schema_rel_dir}")
    
    # 添加前缀子目录（如果你使用了 INCLUDE_PREFIX）
    if (NOT "${FLATBUFFERS_GENERATE_HEADERS_INCLUDE_PREFIX}" STREQUAL "")
        set(current_generated_include_dir "${current_generated_output_dir}/${FLATBUFFERS_GENERATE_HEADERS_INCLUDE_PREFIX}")
    else()
        set(current_generated_include_dir "${current_generated_output_dir}")
    endif()

    # 注意：这里我们使用 --filename-suffix "Flat"，所以文件名是 filename + Flat.h
    set(generated_include "${current_generated_include_dir}/${filename}.h")

    # Add custom command for generation
    add_custom_command(
      OUTPUT "${generated_include}"
      COMMAND ${FLATC}
      -o "${current_generated_include_dir}" # 输出到对应的相对路径子目录
      ${include_params}
      -c "${schema}"
      ${FLATBUFFERS_GENERATE_HEADERS_FLAGS}
      --cpp-ptr-type "stl::unique_ptr"
      --cpp-str-type "stl::string"
      --cpp-str-flex-ctor
      --cpp-std c++11
      --filename-suffix ".flat"
      --cpp-include "Core/Serialization/FlatInclude.h"
      DEPENDS ${FLATC_DEPENDS} "${schema}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" # 使用当前 CMakeLists.txt 所在目录作为工作目录
      COMMENT "Building ${schema} flatbuffers into relative path...")
    
    list(APPEND all_generated_header_files "${generated_include}")
    list(APPEND generated_custom_commands "${generated_include}")

    # Binary schemas generation (逻辑类似，省略细节，确保路径正确即可)
    # ...
  endforeach()

  # Create an additional target as add_custom_command scope is only within same directory (CMakeFile.txt)
  set(generate_target "GENERATE_${FLATBUFFERS_GENERATE_HEADERS_TARGET}")
  add_custom_target(${generate_target} ALL DEPENDS ${generated_custom_commands})

  # Set up interface library
  add_library(${FLATBUFFERS_GENERATE_HEADERS_TARGET} INTERFACE)
  target_sources(${FLATBUFFERS_GENERATE_HEADERS_TARGET} INTERFACE ${all_generated_header_files} ${all_generated_source_files} ${FLATBUFFERS_GENERATE_HEADERS_SCHEMAS})
  add_dependencies(${FLATBUFFERS_GENERATE_HEADERS_TARGET} ${generate_target})
  if(FLATC_DEPENDS)
    add_dependencies(${FLATBUFFERS_GENERATE_HEADERS_TARGET} ${FLATC_DEPENDS})
  endif()

  # 将总的输出根目录添加到 include path 中，这样就可以用相对路径 #include 了
  target_include_directories(${FLATBUFFERS_GENERATE_HEADERS_TARGET} INTERFACE "${generated_target_root_dir}")

  # Organize file layout for IDEs.
  source_group(TREE "${generated_target_root_dir}" PREFIX "Flatbuffers/Generated" FILES ${all_generated_header_files})
  source_group(TREE "${schemas_base_dir}" PREFIX "Flatbuffers/Schemas" FILES ${FLATBUFFERS_GENERATE_HEADERS_SCHEMAS})

endfunction()