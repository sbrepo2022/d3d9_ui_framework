all:
	g++ -c -m32 TestWindowFrame.cpp WindowFrame.cpp SpecialFramework.cpp StrParse.cpp
	g++ -o TestWindowFrame TestWindowFrame.o WindowFrame.o SpecialFramework.o StrParse.o -ld3d9 -ld3dx9 -mwindows -m32