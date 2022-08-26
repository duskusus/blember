import SCons.Tool.compilation_db
env = Environment()
env.Tool('compilation_db')
env.CompilationDatabase()
env.Append(CPPPATH=['#/include', '#/user_objects'])
Export('env')
SConscript('src/SConscript', variant_dir='build', duplicate=0)

