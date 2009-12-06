
project "smartcard++"
  location ("../build/" .. _ACTION)
  uuid "D3E24F38-3540-5D47-9E80-C10FD6AF7A6D"
  language "C++"
  kind     "StaticLib"
  files  { "**.h", "**.cpp" }

  includedirs { "..", "." }
  if isWindows() then
    pchheader("precompiled.h")
    pchsource("precompiled.cpp")
  end

  if not isWindows() then
    includedirs { "/usr/include/PCSC"}
  end

  createConfigs()
