#!/usr/bin/env ruby

ANTIALIAS = 3
N = 20

(0...N).each do |n|
  sld_filename = "sld/mw%02d.sld" % n
  wav_filename = "wav/mw%02d.txt" % n

  deg = 360/20 * n
  rad = deg * Math::PI/180
  x = 30 * Math.cos(rad)
  z = 30 * Math.sin(rad)

  j = (n-10).abs

  ballg = "%3d" % [(j/10.0*255).floor]
  ballr = "%3d" % [((1.0-j/10.0)*255).floor]
  cuberg = "%3d" % [(j/10.0*255).floor]

  File.open(sld_filename, "w") do |f|
    f.puts <<EOL
-200
0 60 -100   30 0
45 45

1  0 2 2 0  100  5 100    0 -30 0   1 0.3 255    0   0 255   0 0 0 0 0
1  5 0 1 0  200 50  5     0  20 100 1 1.0 255  255 255   0   0 0 0 0 0
1  0 0 1 1   15 15  15 #{x}   5 #{z} 1 1.0 255  #{cuberg} #{cuberg} 255   0 #{-deg} 0 0 0 0 0 0
3  0 0 1 0   15 15  15 #{-x}  5 #{-z} 1 1.0 255   #{ballr} #{ballg}   0               0 0 0 0 0
255

255

0 255
1 255
2 255
3 255
255

99 0 1 2 3 255
255

255
EOL
  end

  # w1: 3 in 20
  # w2: 2 in 20
  w1 = 2.0 / 20 * n
  w2 = 1.0 / 20 * n
  
  File.open(wav_filename, "w") do |f|
    f.puts " 30 0 0   15  .1   %.6f" % w1
    f.puts "-30 0 0   60  .1   %.6f" % w2
  end
  
end

(0...N).each do |n|
  sld_filename = "sld/mw%02d.sld" % n
  wav_filename = "wav/mw%02d.txt" % n
  bmp_filename = "bmp/mw%02d.bmp" % n
  system("./raytrace -a#{ANTIALIAS} -q -o#{bmp_filename} -e1#{wav_filename} #{sld_filename}")
end

(0...N).each do |n|
  bmp_from = "bmp/mw%02d.bmp" % n
  bmp_cp1  = "bmp/mw%02d.bmp" % (n+20)
  bmp_cp2  = "bmp/mw%02d.bmp" % (n+40)
  system("cp #{bmp_from} #{bmp_cp1}")
  system("cp #{bmp_from} #{bmp_cp2}")
end
system("cp bmp/mw00.bmp bmp/mw60.bmp")


system('ffmpeg -y -framerate 10 -i bmp/mw%02d.bmp -vf "fps=30" -c:v libx264 -pix_fmt yuv420p mw.mp4')
