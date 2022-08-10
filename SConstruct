env = Environment(CXX='clang++')
env.Append(LIBS=['SDL2', 'GL', 'GLEW', 'GLU'])
env.Append(CCFLAGS='-g')
env.Program('blember', Glob('./*.cpp'))