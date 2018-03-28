###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# GitVersion.cmake
# Dépendances: git
# Variables en sortie :
# ${_prefix}_FOUND
# Si ${_prefix}_FOUND = TRUE:
#   ${_prefix}_STRING
#   ${_prefix}_SHORT
#   ${_prefix}_TINY
#   ${_prefix}_MAJOR
#   ${_prefix}_MINOR
#   ${_prefix}_PATCH
#   ${_prefix}_SHA1

find_package(Git)

#-------------------------------------------------------------------------------
function(GetGitVersion _prefix)

  set (${_prefix}_FOUND FALSE PARENT_SCOPE)
  set (ret 1)

  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe 
      RESULT_VARIABLE ret OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE 
      ERROR_QUIET)
  endif()
  
  if(NOT ret EQUAL 0)
    set(str v1.0-0)
    set(ret 0)
  endif()
  
  #message("GetGitVersion:str=${str}")
  #message("GetGitVersion:ret=${ret}")
  
  if (ret EQUAL 0 AND str)
    
    string(REGEX MATCH "^[vV][0-9]\\.[0-9].*" str ${str})
    #message("GetGitVersion:version=${version}")
    
    if (str)
    
      set (${_prefix}_FOUND TRUE PARENT_SCOPE)
      string(LENGTH "${str}" len)
      math(EXPR len ${len}-1)
      #message("GetGitVersion:len=${len}")

      string(SUBSTRING "${str}" 1 ${len} version_string)
      #message("GetGitVersion:version_string=${version_string}")
      
      string(REGEX REPLACE "[\\.\\-]" ";" version_list "${version_string}")
      #message("GetGitVersion:version_list=${version_list}")

      list(LENGTH version_list len)
      #message("GetGitVersion:len=${len}")
      
      if (${len} GREATER 0)
        list(GET version_list 0 major)
      else(${len} GREATER 0)
        set(major 1)
      endif(${len} GREATER 0)
      #message("GetGitVersion:major=${major}")

      if (${len} GREATER 1)
        list(GET version_list 1 minor)
      else(${len} GREATER 1)
        set(minor 0)
      endif(${len} GREATER 1)
      #message("GetGitVersion:minor=${minor}")

      set(version_tiny "${major}.${minor}")
      
      if (${len} GREATER 2)
        list(GET version_list 2 patch)
        set(version_short "${version_tiny}-${patch}")
      else(${len} GREATER 2)
        set(patch 0)
        set(version_short "${version_tiny}")
      endif(${len} GREATER 2)
      #message("GetGitVersion:patch=${patch}")


      if (${len} GREATER 3)
        list(GET version_list 3 str)
        string(REPLACE "g" "0x" sha1 "${str}")
      else(${len} GREATER 3)
        set(sha1 "0x0")
      endif(${len} GREATER 3)
      #message("GetGitVersion:sha1=${sha1}")

      #message("GetGitVersion:version_string=${version_string}")
      #message("GetGitVersion:version_short=${version_short}")
      #message("GetGitVersion:version_tiny=${version_tiny}")

      set (${_prefix}_STRING ${version_string} PARENT_SCOPE)
      set (${_prefix}_SHORT ${version_short} PARENT_SCOPE)
      set (${_prefix}_TINY ${version_tiny} PARENT_SCOPE)
      set (${_prefix}_MAJOR ${major} PARENT_SCOPE)
      set (${_prefix}_MINOR ${minor} PARENT_SCOPE)
      set (${_prefix}_PATCH ${patch} PARENT_SCOPE)
      set (${_prefix}_SHA1 ${sha1} PARENT_SCOPE)
      unset(version_list)
      unset(version_string)
      unset(version_short)
      unset(version_tiny)
      unset(major)
      unset(minor)
      unset(patch)
      unset(sha1)
      unset(str)
      unset(ret)
      unset(len)
    else (str)
      message(FATAL_ERROR "Unable to find current project version !")
    endif (str)
  else (ret EQUAL 0 AND str)
    message(FATAL_ERROR "Unable to find current project version !")
  endif (ret EQUAL 0 AND str)
endfunction()

#-------------------------------------------------------------------------------
function(WriteGitVersionFile _filename)

  GetGitVersion(WRITE_GIT_VERSION)
  get_filename_component(ext ${_filename} EXT)
  #message("WriteGitVersionFile:ext=${ext}")
  if (${ext} STREQUAL ".h")
    file(WRITE ${_filename}  "#define VERSION \"${WRITE_GIT_VERSION_STRING}\"\n")
    file(APPEND ${_filename} "#define VERSION_SHORT \"${WRITE_GIT_VERSION_SHORT}\"\n")
    file(APPEND ${_filename} "#define VERSION_TINY \"${WRITE_GIT_VERSION_TINY}\"\n")
    file(APPEND ${_filename} "#define VERSION_MAJOR ${WRITE_GIT_VERSION_MAJOR}\n")
    file(APPEND ${_filename} "#define VERSION_MINOR ${WRITE_GIT_VERSION_MINOR}\n")
    file(APPEND ${_filename} "#define VERSION_PATCH ${WRITE_GIT_VERSION_PATCH}\n")
    file(APPEND ${_filename} "#define VERSION_SHA1 ${WRITE_GIT_VERSION_SHA1}\n")
  elseif (${ext} STREQUAL ".mk")
    if (${WRITE_GIT_VERSION_PATCH} EQUAL 0)
      file(WRITE ${_filename}  "${WRITE_GIT_VERSION_MAJOR}.${WRITE_GIT_VERSION_MINOR}\n")
    else()
      file(WRITE ${_filename}  "${WRITE_GIT_VERSION_MAJOR}.${WRITE_GIT_VERSION_MINOR}.${WRITE_GIT_VERSION_PATCH}\n")
    endif()
  else()
    message(WARNING "unsupported file type ${ext}")
  endif()
endfunction()

#-------------------------------------------------------------------------------
