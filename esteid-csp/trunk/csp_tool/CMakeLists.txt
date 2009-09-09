include_directories (./ ../.. pkicommon/)

add_executable(csp_tool 
  csp_tool.cpp precompiled.cpp 
  precompiled.h pkicommon/cryptContext.cpp 
  pkicommon/cryptContext.h pkicommon/helpers.cpp 
  pkicommon/helpers.h)

install(TARGETS csp_tool DESTINATION esteidutil/bin)