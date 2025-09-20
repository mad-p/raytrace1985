# Ray Tracing Around 1985

## Background

Seniors one year above me at TSG (Theoretical Science Group, Univ. of Tokyo)
created a 512 x 512 x 24-bit color frame buffer in 1984.
The main CPU was a 68000, running CP/M 68k on 8-inch floppies.
Scanline renderers and other tools were created to display CG on this frame buffer.

In 1985, there was momentum to create CG animations by frame-recording
what was displayed on that frame buffer onto video.

## The Beginning of Ray Tracing

Professor Tsuyoshi Yamamoto's book contained BASIC ray tracing code
and several sample models.

CQ Publishing "THE 3 DIMENSIONAL COMPUTER GRAPHICS - Practical 3D Graphics on Personal Computers" by Tsuyoshi Yamamoto, 1982

A TSG senior had code that rewrote this BASIC to C.
I was also familiar with the original program itself, having typed in and run
the programs from this book when I was in high school.

## No 8087

PC-9801 of that time ran on Intel 8086. The 8086 had no floating-point arithmetic capability,
which was provided by attaching an external "coprocessor" called the 8087.
PC-9801 also had an 8087 set available, but at about 100,000 yen at the time,
it was too expensive and not available everywhere. On machines without 8087,
floating-point arithmetic was performed in software by hooking interrupts.

Now, when rendering a 512 x 512 image with ray tracing using this C program
without 8087, it took about 10 hours. I received a request from my senior:
"Make it 10 times faster. Without 8087."

To create CG animations, many frames had to be rendered.
Even with simple parallel computation, we needed to borrow PC-9801s
from various research labs in the university at night
(I actually stayed overnight many times exchanging floppy disks).
The 98s we borrowed in this way typically had no 8087, no extended memory, and no HDD.
The system needed to work with just a newly purchased 98 and two 5-inch floppies.

## Making Calculations Faster

Without 8087, I thought that the usual speedup techniques used in ray tracing,
such as bounding boxes or voxel subdivision, would not provide sufficient acceleration.
So I decided on the approach of "creating fast floating-point arithmetic routines
by reducing precision."

I had an idea. Having logarithm and exponent tables, perform multiplication/division
as addition/subtraction of logarithms.
At that time, models were made by combining quadric surfaces (CSG models),
so the square root calculation that appears in the quadratic formula
could be done by "taking logarithm and shifting 1 bit," which was attractive.

Using 64KB logarithm and exponent tables, I created floating-point arithmetic routines
with 15-bit mantissa in assembly language. Initially, the logarithm and exponent tables
were placed in PC-9801's graphics memory.
I later learned that graphics memory was actually disadvantageous for calculation speed
because waits occurred during access due to CRTC also accessing it.

Well, I got carried away and wrote the entire main loop of ray tracing
in assembly language. It's almost completely implemented in assembly routines.
Some parts like file I/O, texture mapping, and bump mapping that required
trigonometric functions were written in C.

The main loop part consists only of integer addition/subtraction and shifts.
As a result, it became 10 times faster than the previous C version,
achieving 8087-equivalent speed.

## Was the Precision Sufficient?

16-bit mantissa, but without using "hidden 1", was effectively 15 bits.
There seemed to be about 5 digits of precision in decimal.
In some models, rays that should hit didn't due to insufficient precision.
This appeared in the rendering results as pixels that should be colored becoming black,
a phenomenon called "black sesame seeds."

Additionally, where objects with mirror-like surface were close together,
infinite reflections occurred, and with the crude color model,
brightness exceeded 1, creating white error pixels.
This was called "white sesame seeds."
In the current C program that accurately reproduces the calculation routines of that time,
the same white and black sesame seeds occur as before.

A model imitating Saturn's rings frequently had black sesame seed phenomena
in the ring parts, creating an effect like real Saturn's rings,
which was sometimes evaluated as being better (this is also reproduced now).

## Animation

The 3D CG animation created was a short story about a dragon being born
and ascending to the sky, drawn through polygon rendering.
Ray tracing didn't have much of a role,
used only in the part where the TSG logo appears in the title and
in the sphere of light that causes halation in the dragon's birth scene.

The frame-recorded animation was completed, and I contributed an article
about the process to PIXEL magazine (Graphics Processing Information Center) at the time.
I also wrote about the white and black sesame seed issues mentioned above.
I think TSG members took turns writing about hardware, dragon movement,
ray tracing, etc. in a short series on the magazine.

The following year, I created a program that could loop animation
by dividing the screen into 20 parts, and by manipulating CRTC registers on the frame buffer.
I had several people create loop animations, and I think they enjoyed it quite well.

## Community

Around 1985, microcomputer clubs at Osaka University and Kyoto University
were also doing CG, and [DoGA](https://doga.jp/2010/general/about.html) was established.
To make it easier to exchange model data for future 3D CG work,
there was an exchange where the Kansai group would draft polygon models
and the Kanto group would draft CSG models suitable for ray tracing.
This connection later led me to participating as a volunteer
in DoGA CGA Contest events.

----
Kaoru Maeda
September 2025
