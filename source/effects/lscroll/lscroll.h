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

#ifndef _LSCROLL_H_
#define _LSCROLL_H_

#include <bmf.h>

// I think in degrees, C thinks in radians, these convert to-and-fro
#ifndef RAD2DEG
#  define RAD2DEG(r)  (r * 57.2957)
#endif

#ifndef DEG2RAD
#  define DEG2RAD(d)  (d *  0.0174532)
#endif

//------------------------------------------------------------------------------
// This is a trivial multi-line Looping_SCROLLer with sine
// This was supposed to be another demo of hooks,
//   but the reultant code was uniterpretable :(
// This code is crude and may need tweaking for other applications

#define lscroll_max  10  // Maximum number of active scrollers


// Initialise loop-scroller subsystem
void  lscroll_init(void) ;

// Configure a scroller
// n     : scroller id
// l     : left boundary
// r     : right boundary
// y     : y coord of text (normally ASCENDER)
// spd   : number of pixels per shift
// sineh : sine height in pixels [wave will pulse up & down by this many pixels]
// sinel : sine length (wavelength, lambda) in pixels
// sinex : start offset in to sine table
// font  : which font to use
// s     : text to display
bool  lscroll_set( int n, int l, int r, int y,
                   int spd, int sineh, f32 sinel, int sinex,
                   BMF_Font* font, char* s ) ;

// Disable a scroller
void  lscroll_clr(int n) ;

// Iterate all active scrollers
void  lscroll_doit(void) ;

#endif // _LSCROLL_H_
