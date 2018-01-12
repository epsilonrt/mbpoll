###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# GitVersion.cmake
# Dépendances: git, git-version
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

  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe RESULT_VARIABLE ret OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
  else(GIT_FOUND)
    set(str v1.0-0)
    set(ret 0)
  endif(GIT_FOUND)
  
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

      if (${len} GREATER 2)
        list(GET version_list 2 patch)
      else(${len} GREATER 2)
        set(patch 0)
      endif(${len} GREATER 2)
      #message("GetGitVersion:patch=${patch}")

      set(version_short "${major}.${minor}-${patch}")
      set(version_tiny "${major}.${minor}")

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

  find_program(GITVERSION NAMES git-version PATHS ${PROJECT_TOOLS_PATHS})
  #message("WriteGitVersionFile:GITVERSION=${GITVERSION}")
  if (GITVERSION)
    execute_process(COMMAND ${GITVERSION} ${_filename})
  endif (GITVERSION)
endfunction()

#-------------------------------------------------------------------------------
