aircraftLayout.ppm		texture ganz normales pixmap
aircraft.mtl			Material für object
aircraft.obj			liste von dreiecke/punkt/normalen expoerteirt von blender
				    muss editiert werden, alle texture files -> ppm, nicht bmp
artificalHorizont.mtl
artificalHorizont.obj
artificalHorizont_UV.ppm

DefosServer-und-2-cleints.cpp	clients schreiben in einen FIFO, server leitet zum visualiere weiter
executeit			compile & execute (deactivate hw beschleuniger, sonst crash)
Helper				includes
Importer			von blender expoert zum opengl representation
Visualisation			interface zu opengl


Note: In case it crashes due to 3d accelarator:
call to execute:
LIBGL_ALWAYS_INDIRECT=1 ./tst_defos

