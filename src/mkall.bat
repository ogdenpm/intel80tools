cd common & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm0 & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm1 & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm2 & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm3 & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm4 & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm5 & ..\..\mkdepend.pl & ..\..\nmake
cd ..\plm6 & ..\..\mkdepend.pl & ..\..\nmake
rem other tools are stored in packed file
cd ..\link80 & ..\..\unpack.pl & ..\..\nmake
cd ..\loc80 & ..\..\unpack.pl & ..\..\nmake
cd ..\asm80 & ..\..\unpack.pl & ..\..\nmake
cd ..\lib80 & ..\..\unpack.pl & ..\..\nmake
cd ..\isis41 & ..\..\unpack.pl & ..\..\nmake
cd ..\isist0 & ..\..\unpack.pl & ..\..\nmake
rem check the builds
rem done separately to avoid errors being lost
rem inside a long build list
cd ..\plm & ..\..\nmake verify
cd ..\plm0 & ..\..\nmake verify
cd ..\plm1 & ..\..\nmake verify
cd ..\plm2 & ..\..\nmake verify
cd ..\plm3 & ..\..\nmake verify
cd ..\plm4 & ..\..\nmake verify
cd ..\plm5 & ..\..\nmake verify
cd ..\plm6 & ..\..\nmake verify
cd ..\link80 & ..\..\nmake verify
cd ..\loc80 & ..\..\nmake verify
cd ..\asm80 & ..\..\nmake verify
cd ..\lib80 & ..\..\nmake verify
cd ..\isis41 & ..\..\nmake verify
cd ..\isist0 & ..\..\nmake verify
cd ..
