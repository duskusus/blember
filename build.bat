clang++ -std=c++14 -g src/*.cpp -I./include/ -L./lib -I./user_objects -lSDL2main -lshell32 -lopengl32 -l./lib/* -lSDL2 -o SDLBase.exe -Xlinker /subsystem:console -m64
