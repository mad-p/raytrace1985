cc1 ray1
if errorlevel 1 goto error1
cc2 ray1
if errorlevel 1 goto error1
cc3 ray1
if errorlevel 1 goto error1
cc4 ray1
if errorlevel 1 goto error1
command /ccc ray3
command /ccc rfconv
command /ccc nomap
command /ccc testmap
copy *.obj ..\obj /y
cd ..\asm
masm ray2;
if errorlevel 1 goto error
masm ray4;
if errorlevel 1 goto error
copy *.obj ..\obj /y
cd ..\obj
marion -u raycore ray2 ray3 ray4 rfconv reala
link tbl+ray1+testmap,raytrace,raytrace/map,raycore+c86s2s
if errorlevel 1 goto error
goto allok
:error1
:allok
copy raytrace.exe ..\sld /y
cd ..\c
