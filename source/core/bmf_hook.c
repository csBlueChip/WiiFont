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
// Set a hook
bool        BMF_hookSet (BMF_Font* font, int hookid, int seq, void*  ptf)
{
    BMF_Hook*  hp;

    if (hookid > HOOK_CNT)  return false ;

    hp = (font) ? (font->hook +hookid) : ( BMFs.hook +hookid) ;
    if (seq)  hp->seq = seq ;
    hp->ptf = ptf;

    return true ;
}

//------------------------------------------------------------------------------
// Get current hook
BMF_Hook*   BMF_hookGet (BMF_Font* font, int hookid, BMF_Hook* hook)
{
    BMF_Hook*  hp;

    if (hookid > HOOK_CNT)  return NULL ;

    hp = (font) ? (font->hook +hookid) : (BMFs.hook +hookid) ;
    if (hook)  memcpy(hook, hp, sizeof(BMF_Hook)) ;

    return hp ;
}

