
EIDCardps.dll: dlldata.obj EIDCard_p.obj EIDCard_i.obj
	link /dll /out:EIDCardps.dll /def:EIDCardps.def /entry:DllMain dlldata.obj EIDCard_p.obj EIDCard_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del EIDCardps.dll
	@del EIDCardps.lib
	@del EIDCardps.exp
	@del dlldata.obj
	@del EIDCard_p.obj
	@del EIDCard_i.obj
