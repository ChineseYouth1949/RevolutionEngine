function(LinkEngine target_name)
    add_dependencies(${target_name} REngine)
    target_link_libraries(${target_name} PUBLIC REngine)

    set(DEPLOY_TARGET_NAME "${target_name}LinkREngine")

    add_custom_target(${DEPLOY_TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "$<TARGET_FILE_DIR:REngine>" "$<TARGET_FILE_DIR:${target_name}>"
        COMMENT "Deploying REngine binaries to $<TARGET_FILE_DIR:${target_name}>"
        VERBATIM
    )

    add_dependencies(${target_name} ${DEPLOY_TARGET_NAME})
    add_dependencies(${DEPLOY_TARGET_NAME} REngine)

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/Resource/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target_name}>/Resource/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Resource/"
            "$<TARGET_FILE_DIR:${target_name}>/Resource/"
        COMMENT "Copying Runtime Resources for ${target_name}"
        VERBATIM
    )
endfunction()