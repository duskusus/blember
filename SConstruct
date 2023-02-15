import SCons.Tool.compilation_db
env = Environment(CXX='clang++')
env.Tool('compilation_db')
env.CompilationDatabase()
env.Append(CPPPATH=['#/include', '#/user_objects'])
env.Append(LIBS=['SDL2', 'GL', 'GLEW', 'GLU', 'm', 'SDL2_ttf'])
env.Append(CCFLAGS=['-g', '-std=c++17', '-O2'])
Export('env')
SConscript('src/SConscript', variant_dir='build', duplicate=0)