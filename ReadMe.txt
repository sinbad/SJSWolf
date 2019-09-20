
                     ****************************************
                     *                                      *
                     *             SJS - WOLF               *
                     *                                      *
                     *         Source Code Release          *
                     *                                      *
                     *      By Steven Streeting, 1995       *
                     *          Envy Technologies           *
                     *                                      *
                     ****************************************
                               E-mail: sinbad@itl.net

INTRODUCTION
============

SJS-Wolf is a demonstration 3D engine I developed over the last few months of
1995. I have decided to release the source code to the engine now, since I
hope it may be of interest / use to some of you out there.

Features of the engine are:

 - 4 degrees of freedom (3 movement, 1 rotation)
 - Perspective-correct texture mapped walls and floor
 - Player-centred light sourcing on walls and floor
 - Support for different viewer heights
 - Controller support for keyboard and 2/3 button mice
 - Collision detection

Basically it's the Wolfenstein 3D engine with a few extra bits (namely the
floor texturing and the lightsourcing). Hell, I never said it was an original
idea.....;) I do think the extra effects make it incomparable to the look
of the ole classic though.

SJS-Wolf was written using Watcom C/C++ 10.0a and Borland Turbo Assembler 4.0,
compiling in 32-bit protected mode under DOS4/GW. This source code should
compile under any C/C++ compiler that supports 32-bit addressing as long as
an appropriate DOS extender is used (e.g. DOS4/GW, Phar-Lap etc). It will not
function correctly under real mode DOS, since all addresses are based on a
linear address space and will have unpredictable results.

HOWEVER - note that all the ASM routines are reliant on REGISTER parameter
passing. If you use a compiler that hasn't got this option and passes parameters
over the stack, the calls to the ASM will not work properly. I don't know if
any compilers other than Watcom support this.

Use of this code - SJSW_SRC.ZIP
================

This source code is provided free of charge. Use as you see fit.

I'm releasing this code in the hope that it's example might help others
trying to start off their own engines. If you find it useful, please greet
me in whatever you produce. While I don't mind people using this code as a
template for their own learning, severely bad Karma will be gained by the
person simply churning out the same code with new graphics and textures and
claiming it's their own work. Lamers, delete this archive now.


As for everybody else, good luck! I hope it's useful.

Disclaimer
==========
This source code is released as-is and is no responibility is accepted by the
author for it's use or misuse of any kind.

Greets
======

I didn't get here on my own either, so here's my credits / greets list for
all those who have helped me get this far either directly or indirectly:

 - Mike Abrash (above all)
 - Alexey Goloshubin 
 - Chris Lampton
 - Billy Zelsnack 
 - S-Cubed
 - Simon Booth
 - Frontman
 - Everyone on comp.graphics.algorithms


Steve Streeting
sinbad@itl.net

END



