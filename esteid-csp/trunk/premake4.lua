
project "esteidcsp"
  uuid "DBCD4071-608C-4443-A440-45E15FE05BC3"
  location ("../../../build/" .. _ACTION)
  language "C++"
  kind     "SharedLib"

  files  { "*.h","*.cpp","esteidcsp.rc","csp.def" }
  pchheader "precompiled.h"
  pchsource "precompiled.cpp"
  
  includedirs {
		 "../.." -- utility
		,"../../.." -- smartcard++ 
		}
  defines {"_WINDOWS","_USRDLL","ESTEIDCSP_EXPORTS","_SCL_SECURE_NO_WARNINGS","_UNICODE","UNICODE"}
  links { "smartcard++" ,"utility"}
  createConfigs()
  configuration "debug"
		       cmds = "..\\..\\..\\cspdk\\cspsign.exe c \"$(TargetPath)\"\r\n"
		cmds = cmds.. "..\\..\\..\\cspdk\\cspsign.exe s \"$(TargetPath)\" \"$(TargetDir)$(TargetName).sig\""
		postbuildcommands(cmds)
