# makefile
# Created by IBM WorkFrame/2 MakeMake at 15:02:38 on 28 Feb 1996
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker

.SUFFIXES: .cpp .obj 

.all: \
    .\login.exe

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Q /Oc /Gm /G4 /Gf /Gi /Gx /C %s

.\login.exe: \
    .\login.obj \
    {$(LIB)}crypt.lib \
    {$(LIB)}tcp32dll.lib \
    {$(LIB)}login.def
    @echo " Link::Linker "
    icc.exe @<<
     /B" /exepack:2 /packd /nologo"
     /Felogin.exe 
     crypt.lib 
     tcp32dll.lib 
     login.def
     .\login.obj
<<

.\login.obj: \
    .\login.cpp
