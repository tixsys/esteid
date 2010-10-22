# Most stuff in this file is stolen from GTK+ jhbuildrc for OS X
# http://github.com/jralls/gtk-osx-build/raw/master/jhbuildrc-gtk-osx

# Determine SDK location
def make_sdk_name(sdk_version):
    return "MacOSX" + sdk_version + ".sdk"

def get_sdkdir(sdk_name):
    return os.path.join("/Developer/SDKs", sdk_name)

# This is the workhorse of the SDK setup.
def setup_sdk(target, sdk_version, architectures):

    os.environ["MACOSX_DEPLOYMENT_TARGET"] = target
    sdkdir = None

    if sdk_version != "native":
        sdkdir = get_sdkdir(make_sdk_name(sdk_version))
        os.environ["MACOSX_SDK_DIR"] = sdkdir

    # Set -isysroot to point compilers to sdkdir
    environ_append("CFLAGS", "-isysroot " + sdkdir)
    environ_append("CXXFLAGS", "-isysroot " + sdkdir)
    environ_append("LDFLAGS", "-isysroot " + sdkdir)

    addpath("PKG_CONFIG_PATH", "/usr/lib/pkgconfig")

    # Set the -arch flags for everything we're building.
    #
    for arch in architectures:
        environ_prepend("CFLAGS", "-arch " + arch)
        environ_prepend("CXXFLAGS", "-arch " + arch)
        environ_prepend("LDFLAGS", "-arch " + arch)

    # Set minimum required OS version flags
    environ_append("CFLAGS", "-mmacosx-version-min=" + target)
    environ_append("CXXFLAGS", "-mmacosx-version-min=" + target)
    environ_append("LDFLAGS", "-mmacosx-version-min=" + target)

    # Make sure builders find libltdl (Anttix)
    os.environ["LTLIB_LIBS"] = sdkdir + "/usr/lib/libltdl.a"

    # If we're building on Snow Leopard for 32-bit, we need to make
    # sure that Perl and Python are working in 32-bit mode.
    #
    if  _osx_version.startswith("10.") and architectures == ["i386"]:
        os.environ["VERSIONER_PERL_PREFER_32_BIT"] = "yes"
        os.environ["VERSIONER_PYTHON_PREFER_32_BIT"] = "yes"

    return sdkdir

# This is the function to call to build universal binaries. By default
# it will build a ppc/i386/x86_64 universal binary for 10.5 and
# 10.6. Arguments are the same as for setup_sdk, but with different
# names. See the notes on 64-bit compilation at setup_sdk as well.
def setup_universal_build(target="10.5", sdk_version="10.5", 
                          architectures=['ppc', 'i386', 'x86_64']): 
    global autogenargs
    global cmakeargs
    _sdkdir = setup_sdk(target, sdk_version, architectures)
    if _sdkdir == None:
        raise Exception("Cross-compiling without specifying an SDK is not supported")
    _s = " "
    os.environ["ARCH"] = _s.join(architectures)

    # Dependency tracking is incompatible with multiarch builds
    autogenargs = autogenargs + "--disable-dependency-tracking"

    # Set up CMAKE
    cmakeargs += " -DCMAKE_OSX_ARCHITECTURES='" + ";".join(architectures) + "'"
    cmakeargs += " -DCMAKE_OSX_SYSROOT=" + _sdkdir
    cmakeargs += " -DCMAKE_OSX_DEPLOYMENT_TARGET=" + target
