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
// Set the current active area
void  BMF_area (const int left, const int top, const int right, const int btm)
{
    BMFs.l = left;
    BMFs.t = top;
    BMFs.r = right;
    BMFs.b = btm;

    BMFs.x = (f32)left;
    BMFs.y = (f32)top;
}

//------------------------------------------------------------------------------
// Do or don't: Display a character if part of it will be out of the active area
void  BMF_halfc (const bool b)
{
    BMFs.halfc = b;
}

//------------------------------------------------------------------------------
// Do or don't: Auto-linefeed when you reach the edge of the active area
void  BMF_autolf (const bool b)
{
    BMFs.autolf = b;
}

//------------------------------------------------------------------------------
//! NEITHER STRING, NOT CHARACTER ALIGNMENT ARE IMPLEMENTED YET
void  BMF_align (const int a)
{
    BMFs.align = a;
}

//------------------------------------------------------------------------------
//! STRING ALIGNMENT IS NOT YET IMPLEMENTED
void  BMF_halign (const int a)
{
    BMFs.align = (BMFs.align & BMF_MASK_VERT) | a;
}

//------------------------------------------------------------------------------
//! CHARACTER ALIGNMENT IS NOT YET IMPLEMENTED
void  BMF_valign (const int a)
{
    BMFs.align = (BMFs.align & BMF_MASK_HORZ) | a;
}
