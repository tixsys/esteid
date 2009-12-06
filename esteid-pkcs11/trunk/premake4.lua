project "esteidpkcs11"
  location ("../../build/" .. _ACTION)
  uuid "AD102339-1537-5F45-A19B-918DF2785A14"
  language "C++"
  kind     "SharedLib"

  files  { "*.h","*.cpp" }
  includedirs { "../.." , ".." }
  if isWindows() then
	includedirs "pkcs11"
	files { "esteidpkcs11.rc" }
  else
    includedirs { "/usr/include/PCSC" }
  end
  if os.is("macosx") then
    includedirs "mac_pkcs11_headers"
  end
    
  links { "smartcardpp" ,"utility"}
  createConfigs()
