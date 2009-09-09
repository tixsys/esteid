
project("csp_tool")
	uuid("92E51541-12B2-478a-A65F-D38E9BF147E7")
	location ("../../../build/" .. _ACTION)
	language "C++"
	kind "ConsoleApp"
	files  { "*.cpp","*.h", "pkicommon/*.cpp", "pkicommon/*.h" }
	includedirs { "." , "pkicommon" }
	createConfigs("StaticRuntime")

