// STEVE'S WOLFENSTEIN ENGINE, Copyright 1995 Envy Technologies
// Main calling demo program
// Version 1.00b
// Lightsourcing!


#include  <stdio.h>
#include  <iostream.h>
#include  <dos.h>
#include  <conio.h>
#include  <stdlib.h>
#include  <math.h>
#include <graph.h>
#include  "sjspcx.h"
#include  "sjstext.h"
#include "sjscreen.h"
#include "sjsfixed.h"
#include "sjskey.h"
#include "sjsmouse.h"


const NUM_IMAGES=15;

pcx_struct textmaps;

char screenbuffer[64000];
extern fixed sin_table[CIRCLE_DIVISIONS];
extern fixed cos_table[CIRCLE_DIVISIONS];

extern char keymap_info[4];
map_type walls={
 { 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 9, 9, 9, 9, 9},
 { 1, 2, 0, 2, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 9},
 { 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 3, 0, 0, 9},
 { 1, 0, 0, 0, 0, 9, 0, 0, 0, 2, 2, 0, 3, 0, 0, 9},
 { 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 2, 3, 0, 8, 9},
 { 1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2, 2, 3, 0, 0, 9},
 { 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 2, 3, 0, 8, 8},
 { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 8},
 { 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 8},
 { 5, 0, 0, 0, 5, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 8},
 { 5, 5, 5, 0, 5, 0, 2, 2, 0, 4, 0, 0, 0, 0, 0, 8},
 { 2, 0, 5, 0, 5, 0, 0, 4, 0, 4, 0, 0, 0, 7, 0, 8},
 { 2, 0, 5, 0, 5, 0, 0, 4, 0, 0, 0, 7, 7, 7, 0, 8},
 { 2, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0, 0, 0, 7, 0, 8},
 { 2, 2, 2, 2, 2, 0, 0, 4, 0, 4, 0, 0, 0, 0, 0, 8},
 { 2, 2, 2, 2, 2, 2, 2, 4, 6, 4, 8, 8, 8, 8, 8, 8}
};


void main()
{
int viewing_angle=0;
fixed xview=(8*64 - 5) << 16;
fixed yview=(8*64) << 16;
int mapx,mapy;
fixed blockx,blocky,remx,remy;
int viewer_height=32;
int exit_game=0;
int do_walls=1,do_floor=1,do_ciel=1;
int mouse_installed;
short int xmouse, ymouse;
int lbutton, mbutton, rbutton;
char keyb;
int ang = 0, movement=0;
int moverate, rotaterate, maxmoverate, maxrotaterate;
Pcx Test;
char *screen=(char *)0xa0000;         //linear address value
unsigned char *text_ptr[NUM_IMAGES];        //array of pointers to textures
int tile;


 // Display the startup message

 cout << "\n\n\n\n\n\n";
 cout << "          Wolfenstein-type engine by Steve Streeting Version 1.0b\n";
 cout << "        -----------------------------------------------------------\n\n";
 cout << "v1.0b UPDATES: - Combo mouse / keyboard interface \n";
 cout << "               - Collision detection\n";
 cout << "               - Textured floor\n";
 cout << "               \n";
 cout << "\nUse cursor keys or mouse to move. Right mouse button moves forward.\n";
 cout << "The 'F' key toggles the floor on/off, and the 'W' key the walls.";
 cout << "\n\n..................................BATTER ANY KEY..............................";
 
 // wait for a key
 keyb=getch();
 

 // Check for a mouse driver
 mouse_installed = CheckMouse();

 cout << "\n\nSJSInitMouse:  ";
 if (mouse_installed)
        cout << mouse_installed << " button mouse detected";
 else
        cout << "No mouse detected";

 //set up trig tables
 trigtables();

 // Load texture map images:
 cout << "\nSJSPcxLoad:  Loading image data..";
 if (Test.load("sjswal~1.pcx",&textmaps)) exit(1);

 //set up the pointer table
// for (tile = 0; tile < 15; tile++)
// {
//     text_ptr[tile]=(unsigned char *)(textmaps.image +
//                              (tile/5)*320*IMAGE_HEIGHT+(tile%5)*IMAGE_WIDTH);
// }
 
     

 // Set up the lighting tables
 setlighting(textmaps.palette);


 //Set up the keyboard handler (no cheat codes yet)
 cout << "\nSJSInitKeyboard:  ";
 installKeyboardHandler();
 cout << "Done";
 //setCodes();
 

 setupSingleHitKeys();

 //setupAliasKeys();

 
 // Set mode 13h:

 setmode(0x13);

 // Set the palette:

 setpalette(textmaps.palette);

 // Clear the screen and the buffer
  
 cls(screen);
 cls(screenbuffer);
  
 // Set up previous map values for collision
 mapx=(xview >> 22); 
 mapy=(yview >> 22);


// The main game loop!!!!!
while(!exit_game)
{
    // Clear out the previous image in the buffer
    cls(screenbuffer);

    // Alter the viewers angle if he's turning
    viewing_angle += ang;
    // Wrap the circle around
    if(viewing_angle < 0)
            viewing_angle += CIRCLE_DIVISIONS;
    if(viewing_angle >= CIRCLE_DIVISIONS)
            viewing_angle -= CIRCLE_DIVISIONS;

    // Draw the maze            
    draw_maze(walls,screenbuffer,xview,yview,viewing_angle,
                viewer_height,(unsigned char *)textmaps.image,
                do_walls,do_floor,do_ciel);
    // Copy the buffer to screen (includes VGA retrace)
    copybuffer(screenbuffer, screen);


    // Keyboard rotation made linear to accomodate mouse / joystick
    ang = 0;

    if (movement > 0)
    {
        xview += (cos_table[viewing_angle] * movement);
        yview += (sin_table[viewing_angle] * movement);
        movement --;

    }
    else if (movement < 0)
    {
        xview += (cos_table[viewing_angle] * movement);
        yview += (sin_table[viewing_angle] * movement);
        movement ++;
    }

    //Check collision
    


    mapx=(xview >> (16+6)); 
    mapy=(yview >> (16+6));
    blockx = xview & 0xffc00000;
    blocky = yview & 0xffc00000;
    remx = xview & 0x3fffff;
    remy = yview & 0x3fffff;
    
    if(walls[mapx-1][mapy] && (remx < 0xc0000))
       remx = 0xc0000;
     
    if(walls[mapx+1][mapy] && (remx > 0x350000))
       remx = 0x350000;

    if(walls[mapx][mapy-1] && (remy < 0xc0000))     //block directly north?
       remy = 0xc0000;

    if(walls[mapx][mapy+1] && (remy > 0x350000))     //block directly south?
       remy = 0x350000;
    

    yview = blocky | remy;
    xview = blockx | remx;
    

    // Check the keys to see what the player is doing

    // Run? - These are the same for now until Collision upgraded
    if(checkKey(KEY_LEFTSHIFT) || checkKey(KEY_RIGHTSHIFT))
    {
        moverate = 2;
        maxmoverate = 8;
        rotaterate = (CIRCLE_DIVISIONS / 120);
        maxrotaterate = (CIRCLE_DIVISIONS / 120);
    }
    // No run
    else
    {
        moverate = 2;
        maxmoverate = 8;
        rotaterate = (CIRCLE_DIVISIONS / 120);
        maxrotaterate = (CIRCLE_DIVISIONS / 60);
    }
    
    if(checkKey(KEY_ESC))
        exit_game = 1;
    if(checkKey(KEY_LEFTARROW) && ang < maxrotaterate)
        ang = rotaterate;
    if (checkKey(KEY_RIGHTARROW) && ang > (0-maxrotaterate))
        ang = -rotaterate;
    if (checkKey(KEY_UPARROW))
        movement += moverate;
    if (checkKey(KEY_DOWNARROW))
        movement -= moverate;
    
    if(checkKey(KEY_F))
        do_floor = do_floor ^ 1;
    if(checkKey(KEY_W))
        do_walls = do_walls ^ 1;
    if(checkKey(KEY_C))
        do_ciel = do_ciel ^ 1;

    singleHitProcess();
    

    // Check the mouse movement
    if(mouse_installed)
    {
        GetMouseMove(&xmouse, &ymouse);
        if (abs(xmouse) > 0)
          ang = -(xmouse >> 1) ;
        if(abs(ymouse) > 0)
          movement += -(ymouse >> 4);
        GetMouseButtons(&lbutton, &mbutton, &rbutton);
        if(rbutton)
         movement += moverate;

     if(movement > maxmoverate)
        movement = maxmoverate;
     if(movement < -maxmoverate)
        movement = -maxmoverate;
          
    }  
            
    
            
}


  // Release memory:

  delete textmaps.image;
  delete screenbuffer;
  // Reset video mode and exit:

  setmode(0x03);
  removeKeyboardHandler();

  cout << "\nCreated with Watcom C/C++ 10.0 and Borland Turbo Assembler 4.0\n\n";
  cout << "An -= Envy Technologies =- project. All rights reserved.\n\n";
  cout << "Last updated 23.47pm 15 December 1995\n\n";
  cout << "Presently I have only marginal plans for this engine - I am now developing\n";
  cout << "a far more advanced engine supporting non-orthoganal walls, different floor\n";
  cout << "heights, proper room-above-room capabilities.\n\n";
  cout << "A full Envy BBS based game may be produced from this engine, but more\n";
  cout << "coders are required to develop it (I'm too busy with the new engine)\n\n";
  cout << "If you live in Guernsey, can code in C/C++/Pascal/ASM and want to get ";
  cout << "\ninvolved but can't or don't want to write your own engine, get in touch with";
  cout << "\nme (Steven Streeting) on Black Ice BBS, either in private mail or the";
  cout << "\nCode Online area, of which I am co-host. I will make the source available.";
  cout << "\n\n\nApologies to 486 owners if this isn't as fast as you were expecting.";
  cout << "\nThe main texturing is VERY optimised but there's room for improvement in";
  cout << "\nother areas; but like I said, the new engine will get those enhancements! ;)\n";
  cout << "\n\nHaving said that, I'm pretty pleased with performance overall...8)\n\n";
  cout << "See ya around...........Steve";
  

  
}
