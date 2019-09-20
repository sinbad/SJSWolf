// SJSMOUSE.CPP
//   Surprisingly enough this deals with the mouse driver!

#include <i86.h>
#include <dos.h>
#include "sjsmouse.h"



// CheckMouse
//    Tests for an installed mouse driver, and returns 0 for no mouse driver, or the
//   number of buttons supported if one is found.

int CheckMouse(void)
{
    int buttons;
    union REGS regs;


    regs.w.ax = 0;
    int386(0x33, &regs, &regs);

    if(regs.w.ax != 0xFFFF)
        return(0);
    else
    {
        if(regs.w.bx == 0x0003)         //Logitech / 3 button-er
                return(3);
        else
                return(2);              //Sommat else - must be 2 buttons
    }
}

// GetMouseMove
//     Returns the X and Y relative movement in mickeys, in the 2 variables supplied

void GetMouseMove(short int *xmove,short int *ymove)
{
    union REGS regs;

    regs.w.ax = 0x0B;
    int386(0x33, &regs, &regs);
    *xmove = (signed int)regs.w.cx;
    *ymove = (signed int)regs.w.dx;
}

// GetMouseButtons
//      Returns on/off status of mouse buttons

void GetMouseButtons(int *lbutton, int *mbutton, int*rbutton)
{
    union REGS regs;
    short int temp;
    
    regs.w.ax = 0x03;
    int386(0x33, &regs, &regs);
    temp = regs.w.bx;

    *lbutton = temp & 0x0001;
    temp = temp >> 1;
    *rbutton = temp & 0x0001;
    temp = temp >> 1;
    *mbutton = temp & 0x0001;
}

    
    

