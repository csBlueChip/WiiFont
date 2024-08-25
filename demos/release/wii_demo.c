/*------------------------------------------------------------------------------
Copyright (c) 2009 BlueChip, Cyborg Systems

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <gccore.h>
#include <wiiuse/wpad.h>

// This demo uses: GRRLIB_Init(), GRRLIB_FillScreen() and GRRLIB_Render()
// The default drawer also relies on an environment as set up by GRRLIB
#include <grrlib.h>

//------------------------------------------------------------------------------
// A little bundle of stuff for basic Wiimote reading

// Wiimote overscan (to allow the cursor to move off the dge of the screen)
#define WOVERX (200)
#define WOVERY (300)

#define WMU_MAX (2)  // How many wiimotes to read

ir_t  ir[WMU_MAX];   // Results of last read

//-------------------------------------
void  winit(void)
{
  int  i;

  WPAD_Init();
  for (i = 0;  i < WMU_MAX;  i++) {
    WPAD_SetVRes(i, rmode->fbWidth +WOVERX, rmode->xfbHeight +WOVERY);
    WPAD_SetDataFormat(i, WPAD_FMT_BTNS_ACC_IR);
  }
}

//-------------------------------------
void  wscan(ir_t* ir)
{
  int  i;

  WPAD_ScanPads();

  for (i = 0;  i <  WMU_MAX;  i++) {
    WPAD_IR(i, &ir[i]);
    ir->x -= WOVERX/2;
    ir->y -= WOVERY/2;
  }
}

//==============================================================================
//==============================================================================

#include <bmf.h>          // BMF core functions

#include <bmf/drawc.h>    // Library of additional drawers
#include <bmf/lscroll.h>  // Looped sine scroller

#include "hooks.h"        // Game(/app/demo) specific function hooks

BMF_Font* f_mmt;          // The font variable - available to all functions
#include  "mmt.h"         // Layout details for the font
#include  "mmt_0.h"       // Page 0 of the font graphics

BMF_Font* f_wl;           // The font variable - available to all functions
#include  "wl.h"          // Layout details for the font
#include  "wl_0.h"        // Page 0 of the font graphics


int  test_font(void)
{
    //---------------------------------
    // Initialise and configure the BMF subsystem
    BMF_Init();
    BMF_halfc(true);             // Allow partially visible (half) characters
    BMF_autolf(false);           // DON'T automatically \n at end-of line
    BMF_area(25, 25, 615, 470);  // Not too close to the edge of the TV

    //---------------------------------
    // Load and configure font
    if ( (f_mmt = BMF_load(mmt_0, mmt)) < 0 )  return 5;  // Load font layout
    BMF_lead   (f_mmt, -2);    // slightly smaller gap between lines
    BMF_track  (f_mmt, 2);     // slightly bigger  gap between letters
    // The # character in this font displays "No" - so we disable it with a hook
    BMF_hookSet(f_mmt, HOOK_FINDC_IN, HS_FNT, missing_hash);
    // Select & configure the "draw to video using GRRLIB & add a shadow" drawer
    BMF_hookSet(f_mmt, HOOK_DRAWC, HS_FNT, drawc_vid_grr_shad);
    dc_vgs_init(f_mmt);  // Initialise the drawer for this font

    //---------------------------------
    // Load and configure font
    if ( (f_wl  = BMF_load(wl_0,  wl )) < 0 )  return 6;  // Load font header
    // Select & configure the "draw to video using GRRLIB & add a shadow" drawer
    BMF_hookSet(f_wl, HOOK_DRAWC, HS_FNT, drawc_vid_grr_shad);
    dc_vgs_init(f_wl);  // Initialise the drawer for this font
    dc_vgs_cfg (f_wl, 2, 2, 0x50);  // (default shadow was: 3, 3, 0x70)
    // Look in hooks.c for an explanation of this hook...
    BMF_hookSet(f_wl, HOOK_PUTC_PRE, HS_FNT_SYS, wl_align);

    //---------------------------------
    // Initialise & configure the looped scrollers
    // The parameters for this library are:
    //   int scrollerID,  int left_boundary, int right_boundary, int y_offset,
    //   int speed, int sineHeight, f32 sineLengthl, int sineXOffset,
    //   BMF_Font* fontID, char* string
    lscroll_init();
    lscroll_set(0, 50, 580, 38,    2, 12, 1.0, 10,    f_mmt,
                "Download the source code for the WiiFont library and this "
                "demo at http://code.google.com/p/wiifont  ... ");
    lscroll_set(1, 50, 580, 399,    2, 12, 1.7, 115,    f_mmt,
                "Written using devkitPro & GRRLIB ... "
                "Press <home> to exit ... ");
    lscroll_set(2, 0, 640, 449,    3, 0, 0.0,  0,    f_mmt,
// enable line-wrap now ;)
"And as with any good demo, here comes a) more text than you\'ll bother reading and b) all the technical crap that you will find out about by looking at this (massive) string in the source code "
"::: FreeType for Wii requires 2Megs of RAM, WiiFont needs less than 13K "
"::: You can have as many fonts loaded as you have spare RAM and each of them can have private and/or shared animations "
"::: The library is written in about 1400 lines of excellently commented 'C' "
"::: There is currently a total of SIX hook points to allow complex and compound real-time font animations "
"::: Support for \\n \\t and \\r (half-height linefeed) "
"::: The \"wobbly\" text is a demo \"hook\" written in 3 (THREE) lines of code (*including* the variable declaration and the return statement) "
"::: The looped-sine-scroller is not an integral part of WiiFont, just an additional block of code you can use.  It can support as many strings as you desire and each can be windowed in a different place on the screen "
"::: In the pipeline are drawers which will allow you to Zoom, Rotate and/or Render text to a texture... Align-to-baseline/ascender/descender is planned ...Proper tab-stop points, Multi-page font support and Native TPL support are also on my todo list "
"::: I\'m bored now and need to sort out clipping before I can release this code - write something cool people :) "
".....................................................................  ");

    //---------------------------------
    // Run the demo
    do {
      GRRLIB_FillScreen(0x8080E0FF);  // Light Blue

      BMF_home();  // put the cursor in the top-left of the current "area"

      BMF_goto(25,90);  // Dodge the scroller at the top

      BMF_puts(f_mmt, "Grab a font from here:  http://www.thfonts.com/\n\r"
                      "Grab the font generator by ");

      BMF_puts(f_wl,  "WitchLord");

      BMF_puts(f_mmt, " from here:\n"
                      "\thttp://www.angelcode.com/products/bmfont/\n\r"
                      "Colour your font in Gimp/Photoshop/<other>\n\r"
                      "RTFM  ( a.k.a. The comments in bmf.h )\n\r"
                      "Take a look at the code for this demo\n\r");

      // Apply the "wobble" animation to this line of text
      BMF_hookSet(f_mmt, HOOK_PUTC_PRE, 0, wobble);
      BMF_puts(f_mmt, "Write something cool and release it on the scene\n\r");
      BMF_hookSet(f_mmt, HOOK_PUTC_PRE, 0, NULL);

      // Iterate the scrollers
      lscroll_doit();

      GRRLIB_Render();  // Display the frame
      wscan(ir);        // Scan the wiimotes
    } while(!(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME));  // Exit on home key

    return 0;
}

//------------------------------------------------------------------------------
int  main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  // Init graphics subsystem
  GRRLIB_Init();

  // Init the Wiimotes
  winit();

  // Call the demo
  exit( test_font() );
}
