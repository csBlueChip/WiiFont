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

#include <string.h>

#include <grrlib.h>

#include <bmf.h>

//------------------------------------------------------------------------------
// This is the State Table - see the header for more details
BMF_State  BMFs;

//------------------------------------------------------------------------------
// Initialise the State Table with some sensible values
void  BMF_Init (void)
{
    int  i;

    // This is the ONLY alignment system currently implemented
    // Well, I say "implemented", it's more like "what happens if you don't
    // implement an alignment system" !
    BMFs.align  = BMF_ASCENT | BMF_LEFT;  // Veritcal and Horizontal Alignment

    BMFs.x      = 0;     // Screen coordinates (pixels; 0,0 is top-left)
    BMFs.y      = 0;     // ...

    BMFs.autolf = false; // Automatic \n at edge of screen
    BMFs.halfc  = true;  // Allow half-characters (characters partially off the screen)

    BMFs.l      = 0;                 // left   edge of display window
    BMFs.r      = rmode->fbWidth;    // right  edge of display window
    BMFs.t      = 0;                 // top    edge of display window
    BMFs.b      = rmode->xfbHeight;  // bottom edge of display window

    for (i = 0;  i < HOOK_CNT;  i++) {
        BMFs.hook[i].seq = HS_DEFAULT;  // Default loaded to a font
        BMFs.hook[i].ptf = NULL;        // No system-level hook
    }

    // Configure the default drawer
    BMFs.hook[HOOK_DRAWC].ptf = BMF_DRAWC_DFLT;
    memset(BMFs.args, 0, BMF_DRAWC_ARG_MAX);

}

//------------------------------------------------------------------------------
// Retrieve a pointer to (and a snapshot of) the current State Table
BMF_State*  BMF_stateGet (BMF_State* state)
{
    // if we were supplied a pointer to a state struct
    // fill it with a snapshot of the current state
    if (state)  memcpy(state, &BMFs, sizeof(BMF_State)) ;

    // Either way, we return a pointer to the live state-variables
    return &BMFs;
}

//------------------------------------------------------------------------------
// Restore a backup of the State Table
void        BMF_statePut (BMF_State* state)
{
    if (state)  memcpy(&BMFs, state, sizeof(BMF_State)) ;
}
