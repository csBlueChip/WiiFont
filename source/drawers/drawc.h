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

#ifndef _DRAWC_LIB_H_
#define _DRAWC_LIB_H_

#include <bmf.h>

// drawc_vid_grr.c
FN_HOOK_DRAWC       (drawc_vid_grr) ;

// drawc_vid_grr_shad.c
// drawc to video using grrlib with shadows
FN_HOOK_DRAWC       (drawc_vid_grr_shad) ;
bool  dc_vgs_init   (BMF_Font* font) ;
bool  dc_vgs_cfg    (BMF_Font* font, const int x, const int y, const int a) ;
void  dc_vgs_shadow (BMF_Font* font, bool b) ;

#endif // _DRAWC_LIB_H_
