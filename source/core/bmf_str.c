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
// Calculate the pixel length of a string
int   BMF_slen (const BMF_Font* font, const char* s)
{
    int       len;
    char      *cp;
    BMF_Char  *chr;

    for(cp = (char*)s, len = 0;  *cp;  cp++)
        if ( (chr = BMF_findc(font, *cp)) )  len += chr->xadv +font->track ;
    return len;
}

//------------------------------------------------------------------------------
// Display a string
int  BMF_puts (const BMF_Font* font_in, const char* s_in)
{
    char  *cp;
    int   w;
    int   len = 0;

//-in
    // We want the input prams as "const"
    //   but we may want to change them in the hooks!  So...
    BMF_Font*  font = (BMF_Font*)font_in;
    char*      s    = (char*)s_in;

    #define go(ptf) ((hook_puts_in)ptf)(&font, &s)
    #define HOOKID   (HOOK_PUTS_IN)
    #define ptfS    ( BMFs.hook[HOOKID].ptf)
    #define ptfF    (font->hook[HOOKID].ptf)
    switch          (font->hook[HOOKID].seq) {
        case HS_SYS_FNT:  if ( ptfS && !go(ptfS) ) break; //else fall-through
        case HS_FNT:      if ( ptfF && !go(ptfF) ) return 0; else break;
        case HS_FNT_SYS:  if ( ptfF && !go(ptfF) ) break ; //else fall-through
        case HS_SYS:      if ( ptfS && !go(ptfS) ) return 0; else break;
        default:          break;
    }
    #undef ptfF
    #undef ptfS
    #undef HOOKID
    #undef go

    for (cp = (char*)s;  *cp;  cp++)
        switch (( w = BMF_putc(font, *cp) )) {
            default:  len += w;  // Track pixel length of string
            case -2:  break;     // putc() said "no char printed"
            case -1:  goto eol;  // putc() said "stop string now"
        }
    eol:

//-out
    #define go(ptf) ((hook_puts_out)ptf)(&font, &s, &len)
    #define HOOKID   (HOOK_PUTS_OUT)
    #define ptfS    ( BMFs.hook[HOOKID].ptf)
    #define ptfF    (font->hook[HOOKID].ptf)
    switch          (font->hook[HOOKID].seq) {
        case HS_SYS_FNT:  if ( ptfS && !go(ptfS) ) break; //else fall-through
        case HS_FNT:      if ( ptfF && !go(ptfF) ) break; else break;
        case HS_FNT_SYS:  if ( ptfF && !go(ptfF) ) break ; //else fall-through
        case HS_SYS:      if ( ptfS && !go(ptfS) ) break; else break;
        default:          break;
    }
    #undef ptfF
    #undef ptfS
    #undef HOOKID
    #undef go

    return len;
}
