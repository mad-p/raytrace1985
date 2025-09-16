#!/usr/bin/env ruby

ANTIALIAS = 3
N = 180

(0...N).each do |n|
  sld_filename = "sld/tron%03d.sld" % n

  deg = 360/N * n
  rad = deg * Math::PI/180
  x = 120 * Math.sin(rad)
  z = -120 * Math.cos(rad) + 50

  vb = deg

  File.open(sld_filename, "w") do |f|
    f.puts <<EOL
-200
#{x} 10 #{z} 15 #{vb}
60 -30

1 0 0 1 0    20 20 65   0 20 45   1 1.0  250 128 210   0  0 0 0 0 0
3 0 0 1 0    25 40 70   0  0 40   1 1.0  250 128 210   0  0 0 0 0 0
3 0 0 1 0     0 30 30   0 -5  0  -1 1.0  250 128 211   0  0 0 0 0 0
1 0 0 1 0    20 10 31   0 -10 80  1 1.0  250 128 211   0  0 0 0 0 0
2 0 0 1 0     0 -1.5 -1 0  0 50   1 1.0  250 128 211   0  0 0 0 0 0
1 0 0 1 0    22 28 29   0 -5  0   1 1.0  250   0 211 211  0 0 0 0 0
3 0 0 1 0    40 28 28   0 -5  0   1 1.0  250   0 211 211  0 0 0 0 0
3 0 0 1 0     0 15 15   0 -5  0  -1 1.0  250   0 211 211  0 0 0 0 0
3 0 0 1 0    15 25 25   0 -5 70   1 1.0  250 211   0   0  0 0 0 0 0
1 0 0 1 0     5 11 45   0 35 40   1 1.0  250 211 128 128  0 0 0 0 0
3 0 0 1 0    30 45 75   0  0 40   1 1.0  250 211 128 128  0 0 0 0 0
1 0 0 1 0    25 41 70   0  5 40   1 1.0  250   0   0   0  0 0 0 0 0
1 5 0 1 0    52  5 100  0 -36 50  1 1.0  250 200 200 200  0 0 0 0 0
3 0 0 1 0    25 10 10   0 -5  0   1 1.0  250 211 128 128  0 0 0 0 0
3 0 0 2 0    25 20 20   0  0 70   1 0.3    0   0 255   0  0 0 0 0 0
255

255

0 1 2 255
3 1 4 255
5 6 7 255
8 255
9 10 255
12 255
13 255
14 255
255

99 0 1 2 3 4 6 255
99 7 5 255
255

EOL
  end

end

(0...N).each do |n|
  sld_filename = "sld/tron%03d.sld" % n
  bmp_filename = "bmp/tron%03d.bmp" % n
  system("./raytrace -a#{ANTIALIAS} -q -o#{bmp_filename} #{sld_filename}")
end

(0...N).each do |n|
  bmp_from = "bmp/tron%03d.bmp" % n
  bmp_cp1  = "bmp/tron%03d.bmp" % (n+N)
  bmp_cp2  = "bmp/tron%03d.bmp" % (n+2*N)
  system("cp #{bmp_from} #{bmp_cp1}")
  system("cp #{bmp_from} #{bmp_cp2}")
end
system("cp bmp/tron000.bmp bmp/tron#{3*N}.bmp")


system('ffmpeg -y -framerate 30 -i bmp/tron%03d.bmp -vf "fps=30" -c:v libx264 -pix_fmt yuv420p tron.mp4')
