
project "esteidcm"
  uuid "8B92E336-86D7-4b2a-9092-142AE7A763FD"
  location ("../../../build/" .. _ACTION)
  language "C++"
  kind     "SharedLib"

  files  { "*.h","*.cpp","esteidcm.rc"}
  pchheader "precompiled.h"
  pchsource "precompiled.cpp"
  
  includedirs { 
		"."
		, "../.." -- utility
		,"../../.." -- smartcard++ 
		}
  defines { "_CRT_SECURE_NO_DEPRECATE","UNICODE" }
  links { "smartcard++" }
  createConfigs()
