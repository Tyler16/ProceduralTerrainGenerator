# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-src")
  file(MAKE_DIRECTORY "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-src")
endif()
file(MAKE_DIRECTORY
  "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-build"
  "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-subbuild/fastsimd-populate-prefix"
  "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-subbuild/fastsimd-populate-prefix/tmp"
  "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-subbuild/fastsimd-populate-prefix/src/fastsimd-populate-stamp"
  "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-subbuild/fastsimd-populate-prefix/src"
  "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-subbuild/fastsimd-populate-prefix/src/fastsimd-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-subbuild/fastsimd-populate-prefix/src/fastsimd-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/tyler/Programming/ProceduralTerrainGenerator/build/_deps/fastsimd-subbuild/fastsimd-populate-prefix/src/fastsimd-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
