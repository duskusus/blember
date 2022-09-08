import SCons.Tool.compilation_db
env = Environment()
env.Tool('compilation_db')
env.CompilationDatabase()
env.Append(CPPPATH=['#/include', '#/user_objects'])
env.Append(LIBS=['SDL2', 'GL', 'GLEW', 'GLU'])
env.Append(CCFLAGS=['-g'])
Export('env')
SConscript('src/SConscript', variant_dir='build', duplicate=0)