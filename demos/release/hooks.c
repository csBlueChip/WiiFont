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

//------------------------------------------------------------------------------
// Some demo hooks

// This hook is a font animation which simply makes text "wobble" on the screen.
// Note: If you display characters in multiples of 5 it will fail!
FN_HOOK_PUTC_PRE(wobble)
{
    static  int  foo = 0;
    *pY += foo++ %5;
    return true;
}

// The mmt font lacks the '#' character,
// This hook is one way you can resolve this problem
FN_HOOK_FINDC_IN(missing_hash)
{
#if 1
    if (*pC == '#')  return false ;  // just don't print # characters
#else
    if (*pC == '#')  *pC = '?' ;     // replace #'s with ?'s
#endif
    return true;
}

// The "alignment" functionality is not in the core library YET
// ...so this adjusts the "wl" font to "baseline align" with the "mmt" font
FN_HOOK_PUTC_PRE(wl_align)
{
    *pY -= 2;  // Notes: ((29-21)-(36-32) -2)
    return true;
}
