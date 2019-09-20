// Keyboard routines
// Thanks to Billy Zelsnack for releasing his sources which saved
//   me a lot of time in this area!


#include<stdio.h>
#include<conio.h>
#include<dos.h>
#include "sjskey.h"
#include "sjscode.h"

char keyAdd=0;
char key[256];
char aliasKey[512];//first 256 are worthless
char singleHitKey[256];
char singleHitOk[256];
char keyCode,tempKeyCode;
char codeBufferCtr=0;
char codeBuffer[16];
void (__interrupt __far *biosKeyboardHandler)();
char keyboardHandlerInstalled=0;
long numCodeWords = 0; // no cheats just yet!!

char kbHit(void)
 {
 long i;
 if(keyboardHandlerInstalled){for(i=0;i<256;i++){if(key[i])return 1;}}
 else {if(kbhit())return 1;}
 return 0;
 }

void waitKey(void)
 {
 if(keyboardHandlerInstalled){while(!kbHit()){}}
 else {getch();}
 }

void clearKeys(void){long i;for(i=0;i<256;i++){key[i]=0;singleHitOk[i]=1;}}

void __interrupt __far keyboardHandler()
 {
 
 keyCode=inp(0x60);
 
 //tempKeyCode=inp(0x61);        //other people have these lines in their code
 //outp(0x61,tempKeyCode|0x80);  //does anybody know why? If you do, 
 //outp(0x61,tempKeyCode&0x7F);  //please send me some mail
 
 outp(0x20,0x20);
 
 if(keyCode==0xE0)
 {
     keyAdd=128;
 }
 else
 {
     if(keyCode>127)
     {
         keyCode &= 127;
         keyCode += keyAdd;
         singleHitOk[keyCode]=1;
         key[keyCode]=0;
         keyCode=0;
     }
     else
     {
         keyCode+=keyAdd;
         if(!key[keyCode])
         {
             if(singleHitKey[keyCode])
             {
                 if(singleHitOk[keyCode])
                 {
                     key[keyCode]=1;
                 }
             }
             else
             {
                 key[keyCode]=1;
             }
             if(singleHitOk[keyCode])
             {
                 singleHitOk[keyCode]=0;
                 codeBuffer[codeBufferCtr]=keyCode;
                 codeBufferCtr++;
                 if(codeBufferCtr>=16)
                        codeBufferCtr=0;
             }
         }
     }
  keyAdd=0;
  }
 }

void installKeyboardHandler(void)
 {
 long i;
 if(keyboardHandlerInstalled)return;
 for(i=0;i<256;i++){key[i]=0;singleHitOk[i]=1;singleHitKey[i]=0;}
 for(i=0;i<512;i++){aliasKey[i]=0;}
 for(i=0;i<16;i++){codeBuffer[i]=0;}
 numCodeWords=0;codeBufferCtr=0;
 biosKeyboardHandler=_dos_getvect(0x09);
 _dos_setvect(0x09,keyboardHandler);
 keyboardHandlerInstalled=1;
 }

void removeKeyboardHandler(void)
 { 
 if(keyboardHandlerInstalled){_dos_setvect(0x09,biosKeyboardHandler);}
 keyboardHandlerInstalled=0;
 }

char *getCodeBuffer(void){return codeBuffer;}

char breakCheck(void)
 {
 long i0,i1;
 //long i;for(i=0;i<16;i++){printf("%d ",codeBuffer[i]);}printf("\n");
 i0=codeBufferCtr-2;if(i0<0)i0=16+i0;
 i1=codeBufferCtr-1;if(i1<0)i1=16+i1;
 if((codeBuffer[i0]==KEY_LEFTCTRL)&&(codeBuffer[i1]==KEY_NUMLOCK))
  {if(!key[KEY_LEFTCTRL]){return 1;}}
 return 0;
 }

char checkKey(short keyNum)
{
    if(keyNum>255)
    {
        keyNum=aliasKey[keyNum];
    }
    if(keyNum==KEY_BREAK)
    {
        return breakCheck();
    }
    else return key[keyNum];
}

void singleHitProcess(void)
 {
 long i;
 for(i=0;i<256;i++)
 {
      if(singleHitKey[i])
      {
         if(key[i])
         {
           key[i]=0;
         }
       }
 }
 }

void toggleSingleHitKey(short keyNum)
 {
 if(keyNum>255){keyNum=aliasKey[keyNum];}
 singleHitKey[keyNum]=1-singleHitKey[keyNum];
 }

void setAliasKey(short alias,char keyNum)
 {
 aliasKey[alias]=keyNum;
 }

void setupSingleHitKeys(void)
{
 toggleSingleHitKey(KEY_F);
 toggleSingleHitKey(KEY_W);
 toggleSingleHitKey(KEY_C);
}




