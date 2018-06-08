# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BINARY_7Z "")
set(CPACK_BINARY_BUNDLE "")
set(CPACK_BINARY_CYGWIN "")
set(CPACK_BINARY_DEB "")
set(CPACK_BINARY_DRAGNDROP "")
set(CPACK_BINARY_FREEBSD "")
set(CPACK_BINARY_IFW "")
set(CPACK_BINARY_NSIS "")
set(CPACK_BINARY_OSXX11 "")
set(CPACK_BINARY_PACKAGEMAKER "")
set(CPACK_BINARY_PRODUCTBUILD "")
set(CPACK_BINARY_RPM "")
set(CPACK_BINARY_STGZ "")
set(CPACK_BINARY_TBZ2 "")
set(CPACK_BINARY_TGZ "")
set(CPACK_BINARY_TXZ "")
set(CPACK_BINARY_TZ "")
set(CPACK_BINARY_WIX "")
set(CPACK_BINARY_ZIP "")
set(CPACK_BUILD_SOURCE_DIRS "C:/Users/Joseph/Desktop/luajit-master/luajit-master;C:/Users/Joseph/Desktop/luajit-master/luajit-master/build")
set(CPACK_CMAKE_GENERATOR "Visual Studio 15 2017")
set(CPACK_COMPONENTS_ALL "Runtime;Library;Header;Data;Documentation;Example;Other")
set(CPACK_COMPONENTS_ALL_SET_BY_USER "TRUE")
set(CPACK_COMPONENT_DATA_DESCRIPTION "Application data. Installed into share/luajit.")
set(CPACK_COMPONENT_DATA_DISPLAY_NAME "luajit Data")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION "Application documentation. Installed into share/luajit/doc.")
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "luajit Documentation")
set(CPACK_COMPONENT_EXAMPLE_DESCRIPTION "Examples and their associated data. Installed into share/luajit/example.")
set(CPACK_COMPONENT_EXAMPLE_DISPLAY_NAME "luajit Examples")
set(CPACK_COMPONENT_HEADER_DESCRIPTION "Headers needed for development. Installed into include.")
set(CPACK_COMPONENT_HEADER_DISPLAY_NAME "luajit Development Headers")
set(CPACK_COMPONENT_LIBRARY_DESCRIPTION "Static and import libraries needed for development. Installed into lib or bin.")
set(CPACK_COMPONENT_LIBRARY_DISPLAY_NAME "luajit Development Libraries")
set(CPACK_COMPONENT_OTHER_DESCRIPTION "Other unspecified content. Installed into share/luajit/etc.")
set(CPACK_COMPONENT_OTHER_DISPLAY_NAME "luajit Unspecified Content")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Executables and runtime libraries. Installed into bin.")
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "luajit Runtime")
set(CPACK_COMPONENT_TEST_DESCRIPTION "Tests and associated data. Installed into share/luajit/test.")
set(CPACK_COMPONENT_TEST_DISPLAY_NAME "luajit Tests")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_GENERATOR "7Z;ZIP")
set(CPACK_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_INSTALLED_DIRECTORIES "C:/Users/Joseph/Desktop/luajit-master/luajit-master;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "")
set(CPACK_INSTALL_PREFIX "C:/Program Files (x86)/luajit")
set(CPACK_MODULE_PATH "C:/Users/Joseph/Desktop/luajit-master/luajit-master/cmake")
set(CPACK_NSIS_DISPLAY_NAME "luajit 2.0.3")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "luajit 2.0.3")
set(CPACK_OUTPUT_CONFIG_FILE "C:/Users/Joseph/Desktop/luajit-master/luajit-master/build/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake/share/cmake-3.11/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "luajit built using CMake")
set(CPACK_PACKAGE_FILE_NAME "luajit-2.0.3-Source")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "luajit 2.0.3")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "luajit 2.0.3")
set(CPACK_PACKAGE_NAME "luajit")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "LuaDist")
set(CPACK_PACKAGE_VERSION "2.0.3")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_RESOURCE_FILE_LICENSE "C:/Program Files/CMake/share/cmake-3.11/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "C:/Program Files/CMake/share/cmake-3.11/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files/CMake/share/cmake-3.11/Templates/CPack.GenericWelcome.txt")
set(CPACK_RPM_PACKAGE_SOURCES "ON")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_7Z "ON")
set(CPACK_SOURCE_CYGWIN "")
set(CPACK_SOURCE_GENERATOR "7Z;ZIP")
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "C:/Users/Joseph/Desktop/luajit-master/luajit-master;/")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "C:/Users/Joseph/Desktop/luajit-master/luajit-master/build/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "luajit-2.0.3-Source")
set(CPACK_SOURCE_RPM "")
set(CPACK_SOURCE_TBZ2 "")
set(CPACK_SOURCE_TGZ "")
set(CPACK_SOURCE_TOPLEVEL_TAG "win32-Source")
set(CPACK_SOURCE_TXZ "")
set(CPACK_SOURCE_TZ "")
set(CPACK_SOURCE_ZIP "ON")
set(CPACK_STRIP_FILES "")
set(CPACK_SYSTEM_NAME "win32")
set(CPACK_TOPLEVEL_TAG "win32-Source")
set(CPACK_WIX_SIZEOF_VOID_P "4")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "C:/Users/Joseph/Desktop/luajit-master/luajit-master/build/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
