/*
 STEVE'S WOLFENSTEIN ENGINE, 1995 Envy Technologies
 The GUTS of the engine!!!

 Trigtables() sets up the trig tables (woah!) for the whole of th engine. Tables
  are fixed length based on the constant CIRCLE_DIVISIONS set at compile time

 SetLighting(..) sets up the lightsourcing palette references

 Draw_maze(..) raycasts an texture maps the current view into a specified memory
  buffer to be later copied to the screen.

*/



#include <stdlib.h>
#include <iostream.h>
#include <math.h>
#include <conio.h>
#include "sjstext.h"
#include "sjsfixed.h"

extern int bugfix;
fixed fisheye_table[320];       //values of cos(column_angle) precalculated
int arctan_table[320];         //values of arctan to work out ray angle (in deg*8)
                               //(INT because they're used only as indexes to the tan
                               //table of slope values)
fixed tan_table[CIRCLE_DIVISIONS];
                        //slopes through all angles. Fixed point so as
                        //to represent the fractions too.

fixed sin_table[CIRCLE_DIVISIONS];       //Table of sin(theta) values, fixed point
fixed cos_table[CIRCLE_DIVISIONS];      //Table of cos(theta) values, fixed point
#define rad2deg(theta, phi) \
        phi=(theta * (CIRCLE_DIVISIONS / 2)) /PI;  

unsigned char light_table[32][256];   //table of lightsourced colours, 32 gradients


// TRIGTABLES
//      Set up cos(arctangent) table for column_angle to use for fish-eye correction
//      Also set up arctangent table itself to pre-calc all the angles across view
//      There are only 320 of these, one for each column of the display.
//      The resulting values of Theta (stored in arctan_table) are to be used
//      as offsets to the main viewer_angle to calculate the slope of a ray
   

void trigtables(void)
{
    int c, v;
    float temp;
    double theta;
    unsigned int ref=0;
    fixed test;

    cout << "\n\nSJSTrig:  Setting up trigonometry tables...";
    v=VIEWER_DISTANCE;
    for (c=-160; c < 160; c++)
    {
        temp = -atan((float)c/v);           //negative since anticlockwise
        rad2deg(temp, arctan_table[ref]);
        fisheye_table[ref] = cos(atan((float)c/v)) * 65536;
        ref++;
    }
    for (theta = 0, ref=0; ref < CIRCLE_DIVISIONS; theta=theta+((2*PI) / (CIRCLE_DIVISIONS)))
    {
        if(theta == (PI/2) || theta == (0.75 * PI))
                tan_table[ref] = -1;  //infinite slope - not used but for completeness
        else
        {
            temp = tan(theta);
            tan_table[ref] = (long)(temp * 65536);
        }
        temp = sin(theta);
        sin_table[ref] = (long)(temp * 65536);
        temp = cos(theta);
        cos_table[ref] = (long)(temp * 65536);
        ref++;
    }
    
                
}
    
// Setlighting
//    Sets up the array light_table to give 32 versions of the palette at each intensity

void setlighting(unsigned char *palette)
{
    unsigned int colour, search, werror, besterror, bestmatch, lightlevel;
    unsigned char redwant, bluewant, greenwant;
    cout << "\nSJSLighting:  Setting up lightsourcing info";

    // Create 32 tables, 1 for each light level
    for (lightlevel = 0; lightlevel < 32; lightlevel++)
    {
        // For each colour at this light level, calculate the desired colour
        for (colour = 0; colour < 256; colour ++)
        {
            besterror = 50000;                   //higher than possible error
            redwant = (palette[(colour*3)] * lightlevel) / 32;
            greenwant = (palette[(colour*3)+1] * lightlevel) / 32;
            bluewant = (palette[(colour*3) +2] * lightlevel) / 32;

            // Search through the palette finding the best fit
            for (search = 0; search < 256; search ++)
            {
                werror = 0;
                werror += abs(redwant - palette[(search*3)]);
                werror += abs(greenwant - palette[(search*3)+1]);
                werror += abs(bluewant - palette[(search*3)+2]);
                if (werror < besterror)
                {
                    bestmatch = search;
                    besterror = werror;
                }
            }

            // Store the reference to the best match found
            light_table[lightlevel][colour] = bestmatch;
        }
        cout << ".";
    }
}

    


// DRAW_MAZE
//      Draws a raycast image in the viewport of the maze represented
//      in array MAP[], as seen from position XVIEW, YVIEW by a
//      viewer looking at angle VIEWING_ANGLE where angle 0 is due
//      east. Angles are arbitrary divisions of a circle.

void draw_maze(map_type map,char *screen,fixed xview, fixed yview,
                int viewing_angle, int viewer_height,unsigned char * textmaps,
                int do_walls,int do_floor,int do_ciel)
{
    int column, row, floorcol, column_angle, ray_angle,left_angle,right_angle;
    int video_off;                   // Pixel y position and offset
    fixed xd,yd;                  // Distance to next wall in x and y
    fixed grid_x,grid_y;          // Coordinates of x and y grid lines
    fixed xcross_x,xcross_y;      // Ray intersection coordinates
    fixed ycross_x,ycross_y;
    fixed rollup_dist;             //rolled up distance as we go
    fixed xdist,ydist;             // Distance to x and y grid lines
    int xmaze,ymaze;              // Map location of ray collision
    int distance;                 // Distance to wall along ray
    int tmcolumn;                 // Column in texture map
    int next_grid_x;              // Add / subtract each jump
    int next_grid_y;
    int height, top, bot, t;      //vertical texture line details
    fixed x, y, xinc, yinc;                  // originating position of ray
    boolean cast_x, cast_y;      //toggles whether an x or y ray is cast at all
    int lightlevel;
    int tile;
    unsigned int tileptr;

    fixed floor_dist;           //Table of distances along floor - max 200 (looking down)
    fixed v_ratio;              //VIEWER_HEIGHT * VIEWER_DISTANCE - constant per frame
    int wall_bots[320];        //Lowest points of walls across the screen
    fixed leftedge_x,leftedge_y,rightedge_x,rightedge_y;




  if(do_walls)
  {

    // Loop through all columns of pixels in viewport:
    for (column=VIEWPORT_LEFT; column<=VIEWPORT_RIGHT; column++)
    {
        // look up angle of ray in relation to centre of view
        column_angle=arctan_table[column];

        // Calculate angle of ray relative to maze coordinates
        ray_angle=(viewing_angle+column_angle) & (CIRCLE_DIVISIONS-1);

  
        // Get slope of ray:
        fixed slope = tan_table[ray_angle];

         // Place origin of ray at viewers position
         x=xview;
         y=yview;

         // work out the gridlines to test; special case horizontal & vertical lines

         //vertical line - upward
         if (ray_angle == (CIRCLE_DIVISIONS * 0.25))
         {
             cast_x = false;
             cast_y = true;
             next_grid_y = (64 * 65536);
         }
         //vertical line - downward
         else if (ray_angle == (CIRCLE_DIVISIONS * 0.75)) 
         {
             cast_x = false;
             cast_y = true;
             next_grid_y = (-1 * 65536);
         }
         //horz line - right
         else if (ray_angle == 0 ) 
         {
             cast_x=true;
             cast_y=false;
             next_grid_x = (64 * 65536);
         }
         //horz line - left
         else if (ray_angle == (CIRCLE_DIVISIONS * 0.5)) 
         {
             cast_x=true;
             cast_y=false;
             next_grid_x = (-1 * 65536);
         }
         // positive x, positive y
         else if (ray_angle < (CIRCLE_DIVISIONS * 0.25) && ray_angle > 0 )
         {
             cast_x=true;
             cast_y=true;
             next_grid_x = (64 * 65536);
             next_grid_y = (64 * 65536);
         }
         // positive x, negative y
         else if (ray_angle < (CIRCLE_DIVISIONS)
                    && ray_angle > (CIRCLE_DIVISIONS * 0.75) ) 
         {
             cast_x=true;
             cast_y=true;
             next_grid_x = (64 * 65536);
             next_grid_y = (-1 * 65536);
         }
         // negative x, positive y
         else if (ray_angle < (CIRCLE_DIVISIONS * 0.5)
                    && ray_angle > (CIRCLE_DIVISIONS * 0.25) ) 
         {
             cast_x=true;
             cast_y=true;
             next_grid_x = (-1 * 65536);
             next_grid_y = (64 * 65536);
         }
         // negative x, negative y
         else // if (ray_angle > (CIRCLE_DIVISIONS * 0.5)
              //      && ray_angle < (CIRCLE_DIVISIONS * 0.75) ) 
         {
             cast_x=true;
             cast_y=true;
             next_grid_x = (-1 * 65536);
             next_grid_y = (-1 * 65536);
         }

         xcross_x = x;    //set up incase any of these values are uninitialised
         xcross_y = y;    //due to vertical and horizontal rays
         ycross_x = x;
         ycross_y = y;
         rollup_dist=0;   //reset the distance the ray has travelled

         // Potentially infinite raycasting loop! ;)
         for (;;)
         {
             if(cast_x)                //cast a ray in the X direction
             {
                 grid_x=(x & 0xffc00000) + next_grid_x;
                 xcross_x= grid_x;
                 xcross_y= y + FixedMul(slope, (grid_x-x));

                 // Get distance to x (vertical) gridline

                 xd=xcross_x-x;
                 xdist=FixedDiv(cos_table[ray_angle], xd);
             }

             if(cast_y)                //cast a ray in the y direction
             {
                 grid_y = (y & 0xffc00000) + next_grid_y;
                 if(cast_x)            //i.e. if not straight up or down
                     ycross_x=x + FixedDiv(slope, (grid_y-y));
                     //never div by 0 since cast_y will be FALSE
                 else
                     ycross_x=x;
                 ycross_y=grid_y;
                 

                 // Get distance to y (horzontal) grid line:

                 yd=ycross_y-y;
                 ydist = FixedDiv(sin_table[ray_angle], yd);
             }

             // If x grid line is closer...
             if (cast_x && (xdist<ydist || !(cast_y)))  //allow for unitialised xdist or ydist
             {
                 // Calculate maze grid coordinates of square:
                 xmaze=xcross_x >> (16+6);       //convert from fixed point and divide by 64   
                 ymaze=xcross_y >> (16+6);
    
                 // Set x and y to point of ray intersection:

                 x=xcross_x;
                 y=xcross_y;

                 // add the distance to the cumulative total

                 rollup_dist += xdist;

                 // Is there a maze cube here? If so, stop looping:

                 if (map[xmaze][ymaze])
                 {
                     // Find relevant column of texture map:
                     tmcolumn = (int)(y >> 16) & 0x3f;
                     break;
                 }
             }

             // If y grid line is closer:
             else
             { 

              // Calculate maze grid coordinates of square:

              xmaze=ycross_x >> (16+6);
              ymaze=ycross_y >> (16+6);

              // Set x and y to point of ray intersection:

              x=ycross_x;
              y=ycross_y;

              // add the distance to the cumulative total

              rollup_dist += ydist;

              // Is there a maze cube here? If so, stop looping:

              if (map[xmaze][ymaze])
              {
                  // Find relevant column of texture map:
                  tmcolumn = (int)(x >> 16) & 0x3f;
                  break;
              }
             }
         }

         // Get distance from viewer to intersection point: (adjust for fish-eye)
         distance=(int)((FixedMul(rollup_dist, fisheye_table[column])) >> 16);

         // fudge divide by zero problems
         if (distance==0) distance=1;


         // Limit casting visibility to 12 squares
         if (distance < (12 * 64))    
         {

           lightlevel = ((768 - distance) * 32) / 768;
           // Calculate visible height of wall:
           height = VIEWER_DISTANCE * WALL_HEIGHT / distance;

           // Calculate bottom of wall on screen:
           bot = (VIEWER_DISTANCE * viewer_height / distance) + VIEWPORT_CENTER;
  
           // Calculate top of wall on screen:
           top = bot - height;
 
           // Initialize temporary offset into texture map:
           t = tmcolumn << 8;

           // If top of current vertical line is outside of
           // viewport, clip it:
           int dheight=height;
           if (top < VIEWPORT_TOP)
           {
               dheight -= (VIEWPORT_TOP - top);
               t += (((VIEWPORT_TOP-top)*WALL_HEIGHT)/height);
               top=VIEWPORT_TOP;
           } 
           // Clip bottom:
           if (bot > VIEWPORT_BOT)
              dheight -= (bot - VIEWPORT_BOT);

           // Record the bottom of the wall
           wall_bots[column] = bot;

           // Point to video memory offset for top of line:
           video_off = top * 320 + column;

           // Which graphics tile are we using?
           tile=map[xmaze][ymaze]-1;

           // Find offset of tile and column in bitmap:
           tileptr=(tile/4)*256*IMAGE_HEIGHT+(tile%4)*IMAGE_WIDTH+t;
           
           // Bresnham's algo used to scale the texture line for each column

           scalecol(height, &textmaps[tileptr], &screen[video_off],
                        dheight, light_table[lightlevel]);
         }

     }
  }


      if(do_floor)
    {
        //set up the similar triangle floor ratio for this frame
        v_ratio = FixedMul(((viewer_height) << 16), (VIEWER_DISTANCE << 16));


        left_angle = (viewing_angle + arctan_table[0]) & (CIRCLE_DIVISIONS-1);
        
        right_angle = (viewing_angle + arctan_table[319]) & (CIRCLE_DIVISIONS-1);


       video_off = (VIEWPORT_CENTER+1) * 320;
       // Init single texture for floor (fudge)
       tile = 9;
       tileptr=(tile/4)* 256 *IMAGE_HEIGHT+(tile%4)*IMAGE_WIDTH;

        for(row = VIEWPORT_CENTER+1; row <= VIEWPORT_BOT; row++)
        {
            floor_dist  = FixedDiv((row - VIEWPORT_CENTER)<<16 , v_ratio);
            floor_dist  = FixedDiv(fisheye_table[0], floor_dist);
            leftedge_x  = FixedMul(cos_table[left_angle], floor_dist) + xview;
            leftedge_y  = FixedMul(sin_table[left_angle], floor_dist) + yview;
            rightedge_x = FixedMul(cos_table[right_angle],floor_dist) + xview;
            rightedge_y = FixedMul(sin_table[right_angle],floor_dist) + yview;
            

       // Texture the floor now!

            xinc = rightedge_x - leftedge_x; 
            yinc = rightedge_y - leftedge_y;
            xinc = FixedDiv( (320 << 16) ,  xinc);
            yinc = FixedDiv( (320 << 16) ,  yinc);
            lightlevel = ((768 - (floor_dist >> 16)) * 32) / 768;
            if (lightlevel < 0)
               lightlevel = 0;
            floorline(leftedge_x << 10, leftedge_y << 10, xinc << 10, yinc << 10, 
                        &textmaps[tileptr], &screen[video_off],
                        light_table[lightlevel], wall_bots, row);
        
            video_off += 320;
       }
    }
   
          
      
  
}
