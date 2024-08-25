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

// These fucntions directly modify the core State Table
extern  BMF_State  BMFs;

//------------------------------------------------------------------------------
// Move the cursor to specified coordinates
// {0,0} is the top-left of the screen
// Coordinates are apecified in pixels
void  BMF_goto (const int x, const int y)
{
    BMFs.x = (f32)x;
    BMFs.y = (f32)y;
}

//------------------------------------------------------------------------------
// Move the cursor to the top-left of the active area
void  BMF_home (void)
{
    BMF_goto(BMFs.l, BMFs.t);  // goto top-left (x,y)
}

//------------------------------------------------------------------------------
// Get the current cursor coordinates
void  BMF_getcur (int* x, int* y)
{
    *x = (int)BMFs.x;
    *y = (int)BMFs.y;
}

//------------------------------------------------------------------------------
// Carriage return  '\n'
// Move the cursor to the left of the active area
//   and move down the screen a full line
int  BMF_cr(const BMF_Font* font)
{
    BMFs.x  = BMFs.l;
    BMFs.y += font->yadv + font->lead;
    return 0;
}

//------------------------------------------------------------------------------
// Linefeed  '\r'
// Advance down the screen 1/2 line
int  BMF_lf(const BMF_Font* font)
{
    BMFs.y += (font->yadv + font->lead) /2;
    return 0;
}
