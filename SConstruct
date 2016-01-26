import os
import platform
import re

#######################################################
# Custom Configure functions
#######################################################
def CheckCommand(context, cmd):
    context.Message('Checking for %s command...' % cmd)
    r = WhereIs(cmd)
    context.Result(r is not None)
    return r

#######################################################
# Initialize our build environment
#######################################################
env = Environment()
Export('env')

#######################################################
# Default target platform
#######################################################
if platform.system() == 'Linux':
    default_target = 'linux'
elif platform.system() == 'Windows':
    default_target = 'win32'
elif platform.system() == 'Darwin':
    default_target = 'darwin'

#######################################################
# Build variables
#######################################################
debug_restrict_options = (
    '0', 'AJ_DEBUG_OFF',
    '1', 'AJ_DEBUG_ERROR',
    '2', 'AJ_DEBUG_WARN',
    '3', 'AJ_DEBUG_INFO',
    '4', 'AJ_DEBUG_DUMP',
    '5', 'AJ_DEBUG_ALL'
)
target_options = [ t.split('.')[-1] for t in os.listdir('.') if re.match('^SConscript\.target\.[-_0-9A-Za-z]+$', t) ]

vars = Variables()
vars.Add(BoolVariable('V',              'Build verbosity',             False))
vars.Add(EnumVariable('TARG',           'Target platform variant',     os.environ.get('AJ_TARG', default_target), allowed_values = target_options))
vars.Add(EnumVariable('VARIANT',        'Build variant',               os.environ.get('AJ_VARIANT', 'debug'),     allowed_values = ('debug', 'release')))
vars.Add(EnumVariable('DEBUG_RESTRICT', 'Set compiled in debug level', os.environ.get('AJ_DEBUG_RESTRICT'),       allowed_values = debug_restrict_options))
vars.Add('CC',  'C Compiler override')
vars.Add('CXX', 'C++ Compiler override')
vars.Add(EnumVariable('NDEBUG', 'Override NDEBUG default for release variant', 'defined', allowed_values=('defined', 'undefined')))
vars.Update(env)
Help(vars.GenerateHelpText(env))

#######################################################
# Setup non-verbose output
#######################################################
if not env['V']:
    env.Replace( CCCOMSTR =     '\t[CC]      $SOURCE',
                 SHCCCOMSTR =   '\t[CC-SH]   $SOURCE',
                 CXXCOMSTR =    '\t[CXX]     $SOURCE',
                 SHCXXCOMSTR =  '\t[CXX-SH]  $SOURCE',
                 LINKCOMSTR =   '\t[LINK]    $TARGET',
                 SHLINKCOMSTR = '\t[LINK-SH] $TARGET',
                 JAVACCOMSTR =  '\t[JAVAC]   $SOURCE',
                 JARCOMSTR =    '\t[JAR]     $TARGET',
                 ARCOMSTR =     '\t[AR]      $TARGET',
                 ASCOMSTR =     '\t[AS]      $TARGET',
                 RANLIBCOMSTR = '\t[RANLIB]  $TARGET',
                 INSTALLSTR =   '\t[INSTALL] $TARGET',
                 WSCOMSTR =     '\t[WS]      $WS' )

#######################################################
# Load target setup
#######################################################
env['build'] = True
env['build_shared'] = False

env.SConscript('SConscript.target.$TARG')

#######################################################
# Build Configuration
#######################################################
config = Configure(env, custom_tests = { 'CheckCommand' : CheckCommand })
found_ws = config.CheckCommand('uncrustify')
env = config.Finish()

#######################################################
# Compilation defines
#######################################################
if env.has_key('DEBUG_RESTRICT'):
    env.Append(CPPDEFINES = { 'AJ_DEBUG_RESTRICT' : env['DEBUG_RESTRICT'] })
if env['VARIANT'] == 'release' and env['NDEBUG'] == 'defined':
    env.Append(CPPDEFINES = [ 'NDEBUG' ])

#######################################################
# Include path
#######################################################
env.Append(CPPPATH = [ '#dist/include' ])

#######################################################
# Process commandline defines
#######################################################
env.Append(CPPDEFINES = [ v for k, v in ARGLIST if k.lower() == 'define' ])

#######################################################
# Install header files
#######################################################
env.Install('#dist/include/ajtcl/hae', env.Glob('inc/*.h'))
env.Install('#dist/include/ajtcl/hae/interfaces/operation', env.Glob('inc/interfaces/operation/*.h'))
env.Install('#dist/include/ajtcl', env.Glob('../../../core/ajtcl/dist/include/ajtcl/*.h'))
env.Install('#dist/include/ajtcl/services', env.Glob('../../base_tcl/dist/include/ajtcl/services/*.h'))
env.Install('#dist/include/ajtcl/services/Common', env.Glob('../../base_tcl/dist/include/ajtcl/services/Common/*.h'))
# Need to force a dpendency here because SCons can't follow nested
# #include dependencies otherwise
env.Depends('#build/$VARIANT', '#dist/include')

#######################################################
# Build the various parts
#######################################################
if env['build']:
    env.SConscript('src/SConscript',       variant_dir='#build/$VARIANT/src',       duplicate = 0)
    env.SConscript('samples/SConscript',   variant_dir='#build/$VARIANT/samples',   duplicate = 0)

#######################################################
# Run the whitespace checker
#######################################################
# Set the location of the uncrustify config file
if found_ws:
    import sys
    sys.path.append(os.getcwd() + '/tools')
    import whitespace

    def wsbuild(target, source, env):
        return whitespace.main([ env['WS'], os.getcwd() + '/tools/ajuncrustify.cfg' ])

    vars = Variables()
    vars.Add(EnumVariable('WS', 'Whitespace Policy Checker', os.environ.get('AJ_WS', 'off'), allowed_values = ('check', 'detail', 'fix', 'off')))

    vars.Update(config.env)
    Help(vars.GenerateHelpText(config.env))

    if env.get('WS', 'off') != 'off':
        env.Command('#ws_ajtcl', '#dist', Action(wsbuild, '$WSCOMSTR'))
