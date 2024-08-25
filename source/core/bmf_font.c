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

/*------------------------------------------------------------------------------
BMFGenerator  http://www.angelcode.com/products/bmfont/

Binary file layout

This section describes the layout of the tags in the binary file format. To
understand what each tag means refer to the file tags section.


The first three bytes are the file identifier and must always be 66, 77, 70, or
"BMF". The fourth byte gives the format version, currently it must be 3.

# Version 1 (introduced with application version 1.8).
# Version 2 (introduced with application version 1.9) added the outline field in
            the infoBlock and the encoded field in the commonBlock.
# Version 3 (introduced with application version 1.10) removed the encoded field
            in the commonBlock, and added the alphaChnl, redChnl, greenChnl,
            blueChnl instead. The size of each block is now stored without
            accounting for the size field itself. The character id in the
            charsBlock and the kerningPairsBlock was increased to 4 bytes to
            support the full unicode range.


Following the first four bytes is a series of blocks with information. Each
block starts with a one byte block type identifier, followed by a 4 byte integer
that gives the size of the block, not including the block type identifier and
the size value.


Block type 1: info
--------------------------------------------------------------------------------
Field        Size  Type  Off  Comment
--------------------------------------------------------------------------------
fontSize      2    int    0   Height in pixels
bitField      1    bits   2   0:smooth, 1:unicode, 2:italic, 3:bold,
                              4:fixedHeigth, 5-7:reserved
charSet       1    uint   3
stretchH      2    uint   4   Height percentage
aa            1    uint   6
paddingUp     1    uint   7   Padding above
paddingRight  1    uint   8   Padding right
paddingDown   1    uint   9   Padding below
paddingLeft   1    uint  10   Padding left
spacingHoriz  1    uint  11   Spacing horizontal
spacingVert   1    uint  12   Spacing vertical
outline       1    uint  13   Outline Thickness (new in v2)
fontName      n+1  str   14   Font Name (NULL terminated string of length n)

This structure gives the layout of the fields. Remember that there should be no
padding between members. Allocate the size of the block using the blockSize, as
following the block comes the font name, including the terminating null char.
Most of the time this block can simply be ignored.


Block type 2: common
--------------------------------------------------------------------------------
Field        Size  Type  Off  Comment
--------------------------------------------------------------------------------
lineHeight      2  uint   0
base            2  uint   2
scaleW          2  uint   4   image width
scaleH          2  uint   6   image height
pages           2  uint   8
bitField        1  bits  10   bits 0-6: reserved, bit 7: packed
alphaChnl       1  uint  11
redChnl         1  uint  12
greenChnl       1  uint  13
blueChnl        1  uint  14


Block type 3: pages
--------------------------------------------------------------------------------
Field        Size     Type     Off  Comment
--------------------------------------------------------------------------------
pageNames    p*(n+1)  strings  0    p null terminated strings, each of length n

This block gives the name of each texture file with the image data for the
characters. The string pageNames holds the names separated and terminated by
null chars. Each filename has the same length, so once you know the size of the
first name, you can easily determine the position of each of the names. The id
of each page is the zero-based index of the string name.


Block type 4: chars
--------------------------------------------------------------------------------
Field        Size  Type  Offset     Comment
--------------------------------------------------------------------------------
id              4  uint  (c*20)+ 0  These fields are repeated until all
                                    characters have been described
x               2  uint  (c*20)+ 4
y               2  uint  (c*20)+ 6
width           2  uint  (c*20)+ 8
height          2  uint  (c*20)+10
xoffset         2  int   (c*20)+12
yoffset         2  int   (c*20)+14
xadvance        2  int   (c*20)+16
page            1  uint  (c*20)+18
chnl            1  uint  (c*20)+19

The number of characters in the file can be computed by taking the size of the
block and dividing with the size of the charInfo structure,
i.e.: numChars = charsBlock.blockSize/20.


Block type 5: kerning pairs
--------------------------------------------------------------------------------
Field        Size  Type  Offset    Comment
--------------------------------------------------------------------------------
first           4  uint  (c*10)+0  These fields are repeated until
                                   all kerning pairs have been described
second          4  uint  (c*10)+4
amount          2  int   (c* 6)+8

This block is only in the file if there are any kerning pairs with amount
differing from 0.
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "peekpoke.h"

#include <grrlib.h>

#include <bmf.h>

// These fucntions directly modify the core State Table
extern  BMF_State  BMFs;

//------------------------------------------------------------------------------
// Create the internal BMF font from the image and header data
BMF_Font*  BMF_load (const void* png, const void* hdr)
{
    BMF_Font*      f;
    int            c, chrh, i, w, h;

    u8*            bp = (u8*)hdr;

    // Check header
    if ((*bp++ != 'B') || (*bp++ != 'M') || (*bp++ != 'F') || (*bp++ != 3))
        return (BMF_Font*)0 ;

    // Block 1 - Font & Image info
    if (*bp++ != 1)  return (BMF_Font*)-1 ;
    c = PEEK32_LE(bp);  bp += 4;
    chrh = PEEK16_LE(bp);
    bp += c;

    // Block 2 - Common character data
    if (*bp++ != 2)  return (BMF_Font*)-2 ;
    c = PEEK32_LE(bp);  bp += 4;
    w = PEEK16_LE(bp +4);
    h = PEEK16_LE(bp +6);
    bp += c;

    // Skip block 3 - page info
    if (*bp++ != 3)  return (BMF_Font*)-3 ;
    bp += 4 +PEEK32_LE(bp);

    // Block 4 - character map data
    if (*bp++ != 4)  return (BMF_Font*)-4 ;

    // How many characters are there in this set?
    c = PEEK32_LE(bp) / BMF_CHRSIZE;  bp += 4;
    if (!c)  return (BMF_Font*)-10 ;

    // Allocate enough memeory to store the font info
    i = sizeof(BMF_Font) +(c *sizeof(BMF_Char));
    if (!( f = malloc(i) ))  return (BMF_Font*)-9 ;
    f->msize  = i;

    // Font header
    f->tex    = GRRLIB_LoadTexture(png);  // Load font texture
    f->w      = w;
    f->h      = h;
    f->pixw   = 1.0 / w;
    f->pixh   = 1.0 / h;

    f->chr    = (BMF_Char*)(f+1);
    f->cnt    = c;

    f->yadv   = chrh;
    f->track  = 0;
    f->lead   = 0;

    for (i = 0;  i < HOOK_CNT;  i++) {
        f->hook[i].seq = BMFs.hook[i].seq;
        f->hook[i].ptf = NULL;
    }

    // Ensure all drawer args are set to 0 (for sanity)
    memset(f->args, 0, BMF_DRAWC_ARG_MAX);

    for (c = 0;  c < f->cnt;  c++)  {
        BMF_Char*  chr = f->chr +c;
        chr->id   = PEEK32_LE(bp);  bp += 4;
        chr->x    = PEEK16_LE(bp);  bp += 2;
        chr->y    = PEEK16_LE(bp);  bp += 2;
        chr->w    = PEEK16_LE(bp);  bp += 2;
        chr->h    = PEEK16_LE(bp);  bp += 2;
        chr->xoff = PEEK16_LE(bp);  bp += 2;
        chr->yoff = PEEK16_LE(bp);  bp += 2;
        chr->xadv = PEEK16_LE(bp);  bp += 2;
        /*chr->page = PEEK8_LE (bp);*/  bp += 1;
        /*chr->chnl = PEEK8_LE (bp);*/  bp += 1;

        chr->idx[BMF_TL].x = (chr->x         ) /(f32)f->tex->w;
        chr->idx[BMF_TL].y = (chr->y         ) /(f32)f->tex->h;
        //chr->idx[BMF_TL].z = 0;

        chr->idx[BMF_TR].x = (chr->x + chr->w) /(f32)f->tex->w;
        chr->idx[BMF_TR].y = chr->idx[BMF_TL].y;
        //chr->idx[BMF_TR].z = 0;

        chr->idx[BMF_BR].x = chr->idx[BMF_TR].x;
        chr->idx[BMF_BR].y = (chr->y + chr->h) /(f32)f->tex->h;
        //chr->idx[BMF_BR].z = 0;

        chr->idx[BMF_BL].x = chr->idx[BMF_TL].x;
        chr->idx[BMF_BL].y = chr->idx[BMF_BR].y;
        //chr->idx[BMF_BL].z = 0;
    }

    // Ignore block 5 - Kerning pairs
    //if (*bp++ != 5)  return (BMF_Font*)-5 ;
    //bp += 4 +PEEK32_LE(bp);

    return f;
}

//------------------------------------------------------------------------------
// Set the tracking on a font
void  BMF_track (BMF_Font* font, const int t)
{
    font->track = t;
}

//------------------------------------------------------------------------------
// Set the leading [led'ing] on a font
void  BMF_lead (BMF_Font* font, const int l)
{
    font->lead = l;
}
