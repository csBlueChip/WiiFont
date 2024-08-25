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

#include <bmf.h>
#include "lscroll.h"

//-------------------------------------
// Functions

//-------------------------------------
// These are the details about a scrolling area
typedef  struct lscroll_s {
    int        l, r;   // Left/Right boundaries
    int        xoff;   // X offset of sinewave
    int        y;      // y offfset of text (see alignment)

    int        spd;
    int        sineh;  // sine height (Ie. +/- sineh pixels)
    f32        sinel;  // sine length (1 = rmdoe->fbWidth)
    int        sinex;  // sine x-offset

    BMF_Font*  font;   // Font to use
    char*      s;      // String to display
    int        slen;   // Length of string (in pixels)
} lscroll_t;

static lscroll_t  lscroll_info[lscroll_max];  // Scroller details
static int        lscroll_id;                 // Active scroller

//-------------------------------------
// Sine wave variance for looped scroller
FN_HOOK_PUTC_PRE(lscroll_sine)
{
    f32  w;
    int  r;
    f32  f;

    lscroll_t*  li = &lscroll_info[lscroll_id];

    w = li->r -li->l +1;               // width
    w = w /li->sinel;                  // Wavelength (Hz)
    r = (*pX -li->l) +li->sinex;       // (relative cursor posn) + x_offset
    f = (360.0 /w) *r;                 // degrees along path
    f = (sin(DEG2RAD(f)) *li->sineh);  // +/- sineh pixels

    *pY += f;

    return true;
}

//-------------------------------------
void  lscroll_init(void)
{
    memset(lscroll_info, 0, lscroll_max *sizeof(lscroll_t));
}

//-------------------------------------
bool  lscroll_set( int n, int l, int r, int y,
                   int spd, int sineh, f32 sinel, int sinex,
                   BMF_Font* font, char* s )
{
    if (n >= lscroll_max)  return false ;

    lscroll_t*  li = &lscroll_info[n];

    li->l     = l;      // left boundary
    li->r     = r;      // right boundary

    li->xoff  = r-l;    // starting x coordinate
    li->y     = y;      // y coordinate

    li->spd   = spd;    // scroll speed (in pixels)
    li->sineh = sineh;  // sine height (+/- this amount of pixels)
    li->sinel = sinel;  // sine length (in pixels)

    li->font  = font;   // font

    li->s     = s;      // string
    li->slen  = BMF_slen(font, s);

    return true;
}

//-------------------------------------
// Disable a scroller
void  lscroll_clr(int n)
{
    lscroll_info[n].font = NULL;
}

//-------------------------------------
void lscroll_doit(void)
{
    int        x1, y1;
    BMF_State  ss;

    BMF_stateGet(&ss);

    BMF_halfc(true);
    BMF_autolf(false);

    for (lscroll_id = 0;  lscroll_id < lscroll_max;  lscroll_id++) {
        lscroll_t*  li = &lscroll_info[lscroll_id];
        BMF_area(li->l,0,  li->r,rmode->xfbHeight);
        if (li->font)  {
            int  atx = li->l +li->xoff;
            BMF_goto(atx, li->y);
            if (li->sineh)
                BMF_hookSet(li->font, HOOK_PUTC_PRE, HS_SYS_FNT, lscroll_sine);
            do {
                BMF_puts(li->font, li->s);
                BMF_getcur(&x1, &y1);
                if (x1 <= li->l)  li->xoff += li->slen ;
            } while(x1 < li->r);
            li->xoff -= li->spd;
            if (li->sineh)
                BMF_hookSet(li->font, HOOK_PUTC_PRE, HS_SYS_FNT, NULL);
        }
    }

    BMF_statePut(&ss); // leave the state-machine as we found it
}


