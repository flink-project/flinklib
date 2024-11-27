function(version_from_git output_variable)
  execute_process(COMMAND git describe --tags --abbrev=0 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE git_tag OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND git rev-list "${git_tag}..HEAD" --count WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE git_commits_since_tag OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(SUBSTRING "${git_tag}.${git_commits_since_tag}" 1 -1 stripped_version)
  set(${output_variable} ${stripped_version} PARENT_SCOPE)
endfunction()
