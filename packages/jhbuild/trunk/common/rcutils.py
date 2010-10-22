# Utility functions
def environ_prepend(key, value, separator=' '):
    old_value = os.environ.get(key)
    if old_value is not None:
        value = value + separator + old_value
    os.environ[key] = value

def environ_append(key, value, separator=' '):
    old_value = os.environ.get(key)
    if old_value is not None:
        value = old_value + separator + value
    os.environ[key] = value

def append_autogenargs(module, args):
    old_value = module_autogenargs.get(module, autogenargs)
    module_autogenargs[module] = old_value + " " + args

def append_cmakeargs(module, args):
    old_value = module_cmakeargs.get(module, cmakeargs)
    module_cmakeargs[module] = old_value + " " + args

#
# Call either setup_debug or setup_release to get the compilation flags.
#

def setup_debug():
    global autogenargs, cmakeargs
    autogenargs = ""
    cmakeargs = ""
    os.environ['CFLAGS'] = "-O0 -ggdb3"
    os.environ['CXXFLAGS'] = "-O0 -ggdb3"

def setup_release():
    global autogenargs, cmakeargs
    autogenargs = ""
    cmakeargs = ""
    os.environ['CFLAGS'] = "-O2"
    os.environ['CXXFLAGS'] = "-O2"
