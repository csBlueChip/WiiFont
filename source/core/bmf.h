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

#ifndef _BMF_H_
#define _BMF_H_

#ifndef PCDEMO
# include <gccore.h>
# include <grrlib.h>  // we use still GRRLIB to convert PNG to TEX at runtime :/
#else
  typedef  unsigned char        u8;
  typedef  unsigned short int   u16;
  typedef  unsigned long int    u32;
  typedef  signed char          s8;
  typedef  signed short int     s16;
  typedef  signed long int      s32;
  typedef  float                f32;
  typedef  struct {int w,h;}    GRRLIB_texImg;  // To make the PC code compile
  typedef  struct {f32 x,y,z;}  guVector;
  typedef  int                  bool;
  enum     {false, true};
#endif

/*
Before I go any further, let me just give props to lemoade` on freenode/##c for
helping me get my head around forward definitions of typedef'ed structs
*/

/*

______________________________________________________ascent (ascender height)
                     #
_____________________#________________________________cap (capital height)
   ######            #
  #      #           #
  #                  #        #
  #                  #
__#__________________#________________________________median (mean line)
   ######    #####   ######   #  # ####   #    #     ^
         #  #     #  #     #  #  ##    #   #  #      |
         #  #     #  #     #  #  #     #    ##       |_x (x-height)
         #  #     #  #     #  #  #     #    ##       |
  #      #  #     #  #     #  #  #     #   #  #      |
___######___######___#_____#__#__#_____#__#____#_____v__baseline
            #
            #
            #
____________#_________________________________________descent (descender height)
                                                    ^
                                                    |-leading [led'ing]
____________________________________________________v_
  #                                           ^
__#___________________________________________|
  #                                           |
  #                                           |
  #        #                                  |
  #                                           |
__#___________________________________________|
  ######   #   #####                          |___ Character Height
  #     #  #  #     #                         |
  #     #  #  #     #                         |
  #     #  #  #     #                         |
  #     #  #  #     #                         |
__#_____#__#__######__________________________|
              #                               |
              #                               |
              #                               |
______________#_______________________________v_


Kerning  is the amount of space around a particular letter
Tracking is extra spacing added to every letter

putc(x,y, c);         ... y ¬ ascent, x ¬ left

puts(x,y, s, align);  ... align = ^{ASCENT*, CAP, MEDIAN, BASELINE, DESCENT} |
                                  ^{LEFT*, RIGHT, CENTRE/CENTER}

*/


/*
From the BMFGenerator documentation ... http://www.angelcode.com/products/bmfont

Block type 4: chars
--------------------------------------------------------------------------------
Field        Size  Type  Offset     Comment
--------------------------------------------------------------------------------
id              4  uint  (c*20)+ 0  unicode ID
x               2  uint  (c*20)+ 4  tl_x of char in image
y               2  uint  (c*20)+ 6  tl_y of char in image
width           2  uint  (c*20)+ 8  width  of char
height          2  uint  (c*20)+10  height of char
xoffset         2  int   (c*20)+12  horz offset from tl_x
yoffset         2  int   (c*20)+14  vert offset from tl_y
xadvance        2  int   (c*20)+16  kerning
page            1  uint  (c*20)+18  ??
chnl            1  uint  (c*20)+19  RGBA channel with char bitmap (unused here)

These fields are repeated until all characters have been described

The number of characters in the file can be computed by taking the size of the
block and dividing with the size of the charInfo structure,
I.e.:  numChars = charsBlock.blockSize / 20.
*/

#define BMF_CHRSIZE  (20) // This is the size of the BMF header character chunk
                          // NOT the size of the BMF_Char struct below!


//------------------------------------------------------------------------------
// Hook Functions defintions

/*
<BlueChip>  I have a void* ptf ...
            I need to cast the pointer and call the function(with, parameters)
            given my function pointer is typedef'ed with "typedef bool(*fn)(int,int);"
            am I correct in saying the way to call my function would be: ((fn)ptf)(1,2);  ??
<kate`>     yes
<lemonade`> or (*(fn)ptf)(1,2), either way.
<BlueChip>  what is the difference between ((fn)ptf)(1,2);  and  (*(fn)ptf)(1,2);  ?
<lemonade`> the first one is a function pointer that you call,
            the next is a function pointer,
              which you dereference to get a function designator,
              which is automatically converted to a function pointer,
              which you call.
<kate`>     axiom: the value of a function is its address
<lemonade`> (*************puts)("oh hai lookit me");
<boris``>   in the same way an array type is replaced with a pointer to the
            first element, a function type is replaced with a pointer to function
<lemonade`> puts("something"); puts is a function designator that is converted
            to a pointer to the puts function, which gets called with one
            arguemnt, a char * to "something".
<BlueChip>  Of course!  <kicks self>
<BlueChip>  My mind didn't (previously) think about that bit!
<BlueChip>  thanks
*/

// What Image format is being used for the texture
typedef  GRRLIB_texImg    BMF_Tex;

// Forward declarations to allow us to define the function pointers
typedef  struct BMF_Char  BMF_Char;
typedef  struct BMF_Font  BMF_Font;

// Each hook point has two hooks: System (BMF)  and  Font (FNT)
// Each font defines whether it calls one or both functions and in what order
typedef enum BMF_HookSeq {
    HS_SYS = 0,
    HS_FNT,
    HS_SYS_FNT,
    HS_FNT_SYS,
} BMF_HookSeq;

#define HS_DEFAULT  HS_FNT_SYS

// These are the hooks which are defined
enum BMF_HookFunctions {
    HOOK_FINDC_IN = 0,
    HOOK_FINDC_OUT,
    HOOK_PUTC_IN,
    HOOK_PUTC_PRE,
    HOOK_PUTC_POST,
    HOOK_PUTC_OUT,
    HOOK_PUTS_IN,
    HOOK_PUTS_OUT,
    HOOK_DRAWC,
    // Add new hooks above this comment
    HOOK_CNT,
    HOOK_SYS,
    HOOK_FONT,
};

typedef struct BMF_Hook {
    // At font-level this controls the hook calling sequence
    // The set held at system-level are used only as defaults for new fonts
    BMF_HookSeq     seq;
    void*           ptf;  // This is the pointer-to-function
} BMF_Hook;

// Function id : findc_in (NULL if not defined)
// Parameters  : active font, ascii value to be searched for
// Timing      : Before the search starts
// Return      : true -> hook completed successfully
//             : If this is the only-or-last hook , false -> terminate search now
//             : If this is the first-of-two hooks, false -> skip second function
typedef  bool(*hook_findc_in)          (BMF_Font**      , char*   );
#define     FN_HOOK_FINDC_IN(h) bool h (BMF_Font** pFont, char* pC)

// Function id : findc_out (NULL if not defined)
// Parameters  : active font, ascii value that was searched for, resultant BMF_Char
// Timing      : When the search has completed
// Return      : true  -> hook completed successfully
//             : false -> return immediately ...If this is the first-of-two
//             :          hooks, the second hook will not be executed
// Notes       : (*pChr) will be NULL if no character was found by the search
typedef  bool(*hook_findc_out)          (BMF_Font**      , char*   , BMF_Char**     );
#define     FN_HOOK_FINDC_OUT(h) bool h (BMF_Font** pFont, char* pC, BMF_Char** pChr)

// Function id : putc_in (NULL if not defined)
// Parameters  : active font, ascii value requested
// Timing      : Before any display calculations
// Return      : true -> function completed successfully
//           **: If this is the only-or-last hook , false -> terminate display now
//             : If this is the first-of-two hooks, false -> skip second function
// Notes       : **putc() will return 0 -> "character displayed, width 0"
typedef  bool(*hook_putc_in)          (BMF_Font**      , char*   );
#define     FN_HOOK_PUTC_IN(h) bool h (BMF_Font** pFont, char* pC)

// Function id : putc_pre (NULL if not defined)
// Parameters  : active font, ascii value to be printed, chosen x,y coords, BMF_Char to be printed
// Timing    ++: After coordinate adjustments
// Return      : true -> function completed successfully
//           **: If this is the only-or-last hook , false -> terminate display now
//             : If this is the first-of-two hooks, false -> skip second function
// Notes       : **putc() will return -2 -> "character not printed, puts() may continue"
//             : ++Not called if : A special character was printed (\n, \t, \r)
//             :                   The requested character was not found in the font
//             :                   The coordinates & settings dictated "don't print"
typedef  bool(*hook_putc_pre)          (BMF_Font**      , char*   , f32*   , f32*   , BMF_Char**     );
#define     FN_HOOK_PUTC_PRE(h) bool h (BMF_Font** pFont, char* pC, f32* pX, f32* pY, BMF_Char** pChr)

// Function id : putc_post (NULL if not defined)
// Parameters  : active font, ascii value that was printed, final x,y coords, BMF_Char that was printed
// Timing      : After the character has been drawn  and  BMF.x has been updated
//             : but  before the system has had a chance to auto-cr (or not)
// Return      : true -> function completed successfully
//           **: If this is the only-or-last hook , false -> terminate display now
//             : If this is the first-of-two hooks, false -> skip second function
// Notes       : **putc() will return hr->xadv -> "character printed, width n"
typedef  bool(*hook_putc_post)          (BMF_Font**      , char*   , f32*   , f32*   , BMF_Char**     );
#define     FN_HOOK_PUTC_POST(h) bool h (BMF_Font** pFont, char* pC, f32* pX, f32* pY, BMF_Char** pChr)

// Function id : putc_out (NULL if not defined)
// Parameters  : active font, character that was searched for, displayed BMF_Char
// Timing      : When the search has completed
// Return      : true  -> hook completed successfully
//             : false -> return immediately ...If this is the first-of-two
//             :          hooks, the second hook will not be executed
// Notes       : (*pChr) will be NULL if no character was found by the search
typedef  bool(*hook_putc_out)          (BMF_Font**      , char*   , BMF_Char**     );
#define     FN_HOOK_PUTC_OUT(h) bool h (BMF_Font** pFont, char* pC, BMF_Char** pChr)

// Function id : puts_in (NULL if not defined)
// Parameters  : active font, pointer to start of string to be displayed
// Timing      : Before the print begins
// Return      : true -> hook completed successfully
//             : If this is the only-or-last hook , false -> do not print the string
//             : If this is the first-of-two hooks, false -> skip second function
typedef  bool(*hook_puts_in)          (BMF_Font**      , char**   );
#define     FN_HOOK_PUTS_IN(h) bool h (BMF_Font** pFont, char** pS)

// Function id : puts_out (NULL if not defined)
// Parameters  : active font, ptr to string displayed, width of output in pixels
// Timing      : When the search has completed
// Return      : true  -> hook completed successfully
//             : false -> return immediately ...If this is the first-of-two
//             :          hooks, the second hook will not be executed
typedef  bool(*hook_puts_out)          (BMF_Font**      , char**   , int*     );
#define     FN_HOOK_PUTS_OUT(h) bool h (BMF_Font** pFont, char** pS, int* pLen)

// Function id : drawc (MUST be defined)
// Parameters  : Destination x,y coords, font, character, void* for other argumens
// Timing      : When the search has completed
// Return      : true  -> hook completed successfully
//             : false -> return immediately ...If this is the first-of-two
//             :          hooks, the second hook will not be executed
typedef  bool(*hook_drawc)          (void*     , f32  , f32  , const BMF_Font*     , const BMF_Char*    );
#define     FN_HOOK_DRAWC(d) bool d (void* args, f32 x, f32 y, const BMF_Font* font, const BMF_Char* chr)

// You MUST specify a default drawer & prototype it
// To override the default drawer, change this line and recompile WiiFont
#define BMF_DRAWC_DFLT     drawc_vid_grr

// This defines the maximum number of bytes you may use to hold the
// arguments for a drawer
// If you need a larger argument structure either:
//   Edit BMF_ARG_MAX and recompile the BMF Library   or
//   Make one of your arguments a pointer to the rest of your args!
// By default the maximum is 32bytes
// ...if you don't understand what "structure packing" is or how it works then
//    follow these simple (Wii) rules (not valid for all other systems):
//      # Do not use arrays[]
//      # If you have more than 8 (that's 32/4) arguments, only use:
//          int, uint, s32, u32, f32, anything*
#define BMF_DRAWC_ARG_MAX  (32)

FN_HOOK_DRAWC (BMF_DRAWC_DFLT) ;


//------------------------------------------------------------------------------
// Font Structures

struct BMF_Char {
    char      argid[4];  // MUST be first

    u32       id;        // Unicode ID
    u16       x;         // X offset to top-left pixel of char in img map
    u16       y;         // Y offset to top-left pixel of char in img map
    u16       w;         // Width
    u16       h;         // Height
    s16       xoff;      // offset from x of left-most pixel
    s16       yoff;      // offset from y of top-most  pixel
    s16       xadv;      // space after character
//    u8        page;      //
//    u8        chnl;      // which channel is the data on (RGBA)
    guVector  idx[4];    // tl, tr, br, bl (x,y) of charcter in tex
};

// Index order (for BMF_Char.idx)
enum {
    BMF_TL = 0,
    BMF_TR,
    BMF_BR,
    BMF_BL,
};

struct BMF_Font {
    int        msize;       // The amount of memory malloc()d to hold the font

    BMF_Tex*   tex;         // The texture
    int        w, h;        // Width & Height
    f32        pixw, pixh;  // Width and Height of a single pixel

    BMF_Char*  chr;         // Pointer to character array
    int        cnt;         // Number of characters in font

    int        yadv;        // pixels to travel for a \n
    int        track;       // Tracking
    int        lead;        // Leading

    BMF_Hook   hook[HOOK_CNT];     // Font-specific function hooks

    // Arguments for font-level drawer
    // If you need a larger argument structure either:
    //   Edit BMF_DRAWC_ARG_MAX and recompile the BMF Library   or
    //   Make one of your arguments a pointer to the rest of your args!
    u8         args[BMF_DRAWC_ARG_MAX];
};


//------------------------------------------------------------------------------
// State Table
typedef  struct BMF_State {
    f32       x;       // = 0;
    f32       y;       // = 0;

    bool      autolf;  // = false;  // Automatic \n at edge of screen
    bool      halfc;   // = true;   // Allow half-characters
    int       align;   // = BMF_ASCENT;

    int       l;       // = 20;     // left   edge of window
    int       r;       // = 620;    // right  edge of window
    int       t;       // = 20;     // top    edge of window
    int       b;       // = 460;    // bottom edge of window

    BMF_Hook  hook[HOOK_CNT];  // System-global function hooks

    // Arguments for system-level drawer
    // If you need a larger argument structure either:
    //   Edit BMF_ARG_MAX and recompile the BMF Library   or
    //   Make one of your arguments a pointer to the rest of your args!
    u8         args[BMF_DRAWC_ARG_MAX];
} BMF_State;

// Alignment values & masks
#define BMF_MASK_VERT  (0xF8)
#define BMF_ASCENT     (0x80)
#define BMF_CAP        (0x40)
#define BMF_MEDIAN     (0x20)
#define BMF_BASELINE   (0x10)
#define BMF_DESCENT    (0x08)

#define BMF_MASK_HORZ  (0x07)
#define BMF_LEFT       (0x04)
#define BMF_RIGHT      (0x02)
#define BMF_CENTRE     (0x01)
#define BMF_CENTER     (BMF_CENTRE)  // gotta be done!


//------------------------------------------------------------------------------
// Function Prototypes

//-------------------------------------
// System Core - bmf_core.c

// Initialise the font subsystem
void       BMF_Init      (void) ;

// Get a handle to the state table
// Will also take a snapshot of the state table if passed a NON-NULL pointer
BMF_State* BMF_stateGet (BMF_State* state) ;

// Write back a previously saved state (aka state-snapshot)
void       BMF_statePut (BMF_State* state) ;

//-------------------------------------
// Hooks - bmf_hook.c

// Setting hooks.  Parameetrs are:
//   hook location ... pointer to BMF_Font  or  NULL if setting the system hook
//   hook point    ... Eg. HOOK_PUTC_PRE or HOOK_FINDC_IN
//   hook calling sequence ... {0, HS_SYS, HS_FNT, HS_SYS_FNT, HS_FNT_SYS)
//                             where 0 implies "leave unchanged"
//   pointer to your new hook function
// Returns false if it fails
bool        BMF_hookSet (BMF_Font* font, int hookid, int seq, void*  ptf) ;

// Get a handle to a specified hook
// Will also take a snapshot of the hook if passed a NON-NULL pointer
BMF_Hook*   BMF_hookGet (BMF_Font* font, int hookid, BMF_Hook* hook) ;

//-------------------------------------
// Fonts - bmf_font.c

// Load a font header
// Returns N > 0 will be a pointer to the font
//         (-9 <= N <= 0) to signify an error
//         As the font is >10bytes long, using -10 as an error is acceptable
//         0              -> Bad font header
//         -1,-2,-3,-4,-5 -> Block[abs(N)] has a bad block signature
//         -9             -> malloc() failed
BMF_Font*  BMF_load      (const void* png, const void* hdr) ;

// Set Leading (see explanation at top of this file)
void       BMF_lead      (BMF_Font* font, const int l) ;

// Set Tracking (see explanation at top of this file)
void       BMF_track     (BMF_Font* font, const int t) ;

//-------------------------------------
// Strings - bmf_str.c

// Get the pixel-length/width of a string
int        BMF_slen      (const BMF_Font* font, const char* s) ;

// put a string at the current cursor coordinates
// returns the length of the portion of the string that was processed
int        BMF_puts      (const BMF_Font* font, const char* s) ;

//-------------------------------------
// Characters - bmf_chr.c

// put a character at the current cursor coordinates
// \n -> crlf ... \r -> 0.5*lf ... \t -> |mmm|
// If your font does not contain the character 'm', tabs will not work!
int        BMF_putc      (const BMF_Font* font, const char c) ;

// Find a character (used internally)
BMF_Char*  BMF_findc     (const BMF_Font* font, const char c) ;

//-------------------------------------
// Layout  - bmf_layout.c

// Set the working text area
void       BMF_area      (const int left,  const int top,    // tl_x, tl_y
                          const int right, const int btm) ;  // br_x, br_y

// Allow partial characters to be draw near the clipping border
void       BMF_halfc     (const bool b) ;

// Enable auto-lf at end-of-line
void       BMF_autolf    (const bool b) ;

// Set vertical & horizontal alignment
void       BMF_align     (const int a) ;

// Change horizontal alignment
void       BMF_halign    (const int a) ;

// Change vertical alignment
void       BMF_valign    (const int a) ;


//-------------------------------------
// Cursor - bmf_cursor.c

// Move cursor to specific pixel coordinates (0,0 is at the top-left)
void       BMF_goto      (const int x, const int y) ;

// Move the cursor to the top-left of the current area
void       BMF_home      (void) ;

// Retrieve the cursor coordinates
void       BMF_getcur    (int* x, int* y) ;

// Move the cursor to the start of the next line
int        BMF_cr        (const BMF_Font* font) ;

// Move the cursor down the screen by 1/2 line
int        BMF_lf        (const BMF_Font* font) ;

#endif //_BMF_H_
