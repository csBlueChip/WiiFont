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
// Find the requested character within the specified font
BMF_Char*  BMF_findc (const BMF_Font* font_in, const char c_in)
{
    BMF_Char  *chr;
    int       l, r, n;

    // We want the input prams as "const"
    //   but we may want to change them in the hooks!  So...
    BMF_Font*  font = (BMF_Font*)font_in;
    char       c    = (char)c_in;

//-in
    #define go(ptf) ((hook_findc_in)ptf)(&font, &c)
    #define HOOKID   (HOOK_FINDC_IN)
    #define ptfS    ( BMFs.hook[HOOKID].ptf)
    #define ptfF    (font->hook[HOOKID].ptf)
    switch          (font->hook[HOOKID].seq) {
        case HS_SYS_FNT:  if ( ptfS && !go(ptfS) ) break; //else fall-through
        case HS_FNT:      if ( ptfF && !go(ptfF) ) return NULL; else break;
        case HS_FNT_SYS:  if ( ptfF && !go(ptfF) ) break ; //else fall-through
        case HS_SYS:      if ( ptfS && !go(ptfS) ) return NULL; else break;
        default:          break;
    }
    #undef ptfF
    #undef ptfS
    #undef HOOKID
    #undef go

    // Binary chop to find the character we want
    l   = 0;             // left
    r   = font->cnt -1;  // right
    chr = font->chr;     // in case there's only 1 character
    while(l != r) {
        n   = l + ((r - l) / 2);          // mid point
        chr = font->chr +n;
        if      (chr->id > c)  r = n -1;  // lower half
        else if (chr->id < c)  l = n +1;  // upper half
        else                   break ;    // found it
    }
    if (l==r)  chr = font->chr +l ;   // was it the worst-case scenario
    if (chr->id != c)  return NULL ;  // We didn't find the char

//-out
    #define go(ptf) ((hook_findc_out)ptf)(&font, &c, &chr)
    #define HOOKID   (HOOK_FINDC_OUT)
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

    return chr;
}

//------------------------------------------------------------------------------
// Display a single character on the screen
// This block of code looks after "auto-lf" and "halfc" considerations
int  BMF_putc (const BMF_Font* font_in, const char c_in)
{
    BMF_Char  *chr;
    f32       x, y;

//-in
    // We want the input prams as "const"
    //   but we may want to change them in the hooks!  So...
    BMF_Font*  font = (BMF_Font*)font_in;
    char       c    = (char)c_in;

    #define go(ptf) ((hook_putc_in)ptf)(&font, &c)
    #define HOOKID   (HOOK_PUTC_IN)
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

    // Requested Newline
    if (c == '\n')  return BMF_cr(font) ;  // returns 0

    // Requested half-Newline
    if (c == '\r')  return BMF_lf(font) ;  // returns 0

///! I'd like to see tab stop definitions rather than an arbitrary shift amount
    // Requested tab
    if (c == '\t')  {
        int  tab;
        if ( !(chr = BMF_findc(font, 'm')) )  return 0 ;
        tab = 3 *chr->xadv;
        BMFs.x += tab;
        return tab;
    }

    // Try to find the chr we want, bail now if we fail
    if ( !(chr = BMF_findc(font, c)) )  return 0 ;

    // --------
    // Should we even try to print the character
    // -1 => give up;  -2 => a future \n may fix the problem
    // --------
    // NOTE: The order of these checks is critical!!
    if (!BMFs.halfc) {  // ? half-characters are BANNED
        // ? off the top
        if (BMFs.y < BMFs.t)
            /*if (BMFs.Autolf)     // get on the screen
                do  BMF_cr(font);  while(BMFs.BMFy < 0) ;
            else  */return -2 ;    // wait for \n

        // ? off the left
        if        (BMFs.x < BMFs.l) {           // string may reach screen
            BMFs.x += chr->xadv +font->track;
            return -2;
        // ? off the right
        } else if (BMFs.x +chr->xadv > BMFs.r) {
            if (BMFs.autolf)  BMF_cr(font) ;    // auto-lf
            else              return -2 ;       // wait for \n
        }

        // ? off the bottom
        if (BMFs.y +font->yadv > BMFs.b)  return -1 ; // give up now!

    } else {  // half-characters allowed
        // ? off the top
        if (BMFs.y < (BMFs.t -font->yadv))
            /*if (BMFs.Autolf)     // get on the screen
                do  BMF_cr(font);  while(BMFs.y < (-font->yadv)) ;
            else  */return -2 ;    // wait for \n

        // ? off the left
        if        (BMFs.x < (BMFs.l -chr->xadv)) {  // string may reach screen
            BMFs.x += chr->xadv +font->track;
            return -2;
        // ? off the right
        } else if (BMFs.x > BMFs.r) {
            if (BMFs.autolf)  BMF_cr(font) ;        // auto-lf
            else              return -2 ;           // wait for \n
        }

        // ? off the bottom
        if (BMFs.y > BMFs.b)  return -1 ;  // give up now!
    }

//-lf
    // We may wish to tweak the x & y without changing the BMF_State table
    x = BMFs.x;
    y = BMFs.y;

    #define go(ptf) ((hook_putc_pre)ptf)(&font, &c, &x, &y, &chr)
    #define HOOKID   (HOOK_PUTC_PRE)
    #define ptfS    ( BMFs.hook[HOOKID].ptf)
    #define ptfF    (font->hook[HOOKID].ptf)
    switch          (font->hook[HOOKID].seq) {
        case HS_SYS_FNT:  if ( ptfS && !go(ptfS) ) break; //else fall-through
        case HS_FNT:      if ( ptfF && !go(ptfF) ) return -2; else break;
        case HS_FNT_SYS:  if ( ptfF && !go(ptfF) ) break ; //else fall-through
        case HS_SYS:      if ( ptfS && !go(ptfS) ) return -2; else break;
        default:          break;
    }
    #undef ptfF
    #undef ptfS
    #undef HOOKID
    #undef go

    // If we got this far, print the character, already! :)
    // Try to use first the font drawer then the system drawer
    if      (font->hook[HOOK_DRAWC].ptf)
        ((hook_drawc)font->hook[HOOK_DRAWC].ptf)(font->args, x, y, font, chr) ;
    else if (BMFs. hook[HOOK_DRAWC].ptf)
        ((hook_drawc)BMFs. hook[HOOK_DRAWC].ptf)(BMFs .args, x, y, font, chr) ;
    else    return -1;  // No drawers defined - give up!

    BMFs.x += chr->xadv +font->track;  // Advance the cursor

//-disp
    #define go(ptf) ((hook_putc_post)ptf)(&font, &c, &x, &y, &chr)
    #define HOOKID   (HOOK_PUTC_POST)
    #define ptfS    ( BMFs.hook[HOOKID].ptf)
    #define ptfF    (font->hook[HOOKID].ptf)
    switch          (font->hook[HOOKID].seq) {
        case HS_SYS_FNT:  if ( ptfS && !go(ptfS) ) break; //else fall-through
        case HS_FNT:      if ( ptfF && !go(ptfF) ) goto end; else break;
        case HS_FNT_SYS:  if ( ptfF && !go(ptfF) ) break ; //else fall-through
        case HS_SYS:      if ( ptfS && !go(ptfS) ) goto end; else break;
        default:          break;
    }
    #undef ptfF
    #undef ptfS
    #undef HOOKID
    #undef go

    // If we want auto-lf, then check if we are off the side of the screen now
    if (BMFs.autolf && (BMFs.x > BMFs.r))  BMF_cr(font) ;

//-out
    #define go(ptf) ((hook_putc_out)ptf)(&font, &c, &chr)
    #define HOOKID   (HOOK_PUTC_OUT)
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

end:
    return chr->xadv +font->track;
}

