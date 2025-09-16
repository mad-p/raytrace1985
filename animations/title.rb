#!/usr/bin/env ruby

ANTIALIAS = 3
N = 20

(0...N).each do |n|
  sld_filename = "sld/title%02d.sld" % n
  wav_filename = "wav/wave%02d.txt" % n

  # g \/
  # b /\
  j = (n-10).abs
  g = "%3d" % [(j/10.0*255).floor]
  b = "%3d" % [((1.0-j/10.0)*255).floor]

  File.open(sld_filename, "w") do |f|
    f.puts <<EOL
-200
0 200 -193   49.4 0
45 45

1  0 2 2 0  200  5 200    0 -30 0   1 0.3 255    0   0 255   0 0 0 0 0
1  0 0 1 0  200 50   5    0  20 250 1 1.0 255    0 #{g} #{b}   0 0 0 0 0
255

255

0 255
1 255
255

99 0 1 255
255

255
EOL
  end

  # w1: 3 in 20
  # w2: 2 in 20
  w1 = 3.0 / 20 * n
  w2 = 2.0 / 20 * n
  
  File.open(wav_filename, "w") do |f|
    f.puts " 30 0 0   40  .18  %.6f" % w1
    f.puts "-30 0 0   60  .25  %.6f" % w2
  end
  
end

(0...N).each do |n|
  sld_filename = "sld/title%02d.sld" % n
  wav_filename = "wav/wave%02d.txt" % n
  bmp_filename = "bmp/title%02d.bmp" % n
  system("./raytrace -a#{ANTIALIAS} -q -o#{bmp_filename} -e1#{wav_filename} #{sld_filename}");
end

(0...N).each do |n|
  bmp_from = "bmp/title%02d.bmp" % n
  bmp_cp1  = "bmp/title%02d.bmp" % (n+20)
  bmp_cp2  = "bmp/title%02d.bmp" % (n+40)
  system("cp #{bmp_from} #{bmp_cp1}")
  system("cp #{bmp_from} #{bmp_cp2}")
end
system("cp bmp/title00.bmp bmp/title60.bmp")


system('ffmpeg -y -framerate 10 -i bmp/title%02d.bmp -vf "fps=30" -c:v libx264 -pix_fmt yuv420p title.mp4')
