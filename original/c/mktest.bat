command /ccc testreal
del ..\obj\testreal.obj
copy testreal.obj ..\obj
cd ..\obj
link tbl+reala+testreal,testreal,testreal/map,c86s2s
