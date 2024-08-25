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

#include <grrlib.h>
#include <bmf.h>

//------------------------------------------------------------------------------
// These are the arguments required by this drawer
// Do not edit the first or last lines of this definition
//   but put whatever parameters you like in the list
typedef struct DrawcArgs {
    bool  on;  // Shadow on/off
    int   x;   // x offset
    int   y;   // y offset
    int   a;   // Alpha level
} DrawcArgs;

// Do not edit this
static  DrawcArgs*  arg;

//------------------------------------------------------------------------------
// This initialises the drawer
// The first (and only) parameter is a BMF_Font*
// ...if this is NULL, the settings will be applied to the system-level drawer
// You will need to change the "vgs" to a nice ID for your drawer
// You may wish to replace the "void" with an argument list
// It may return false if there is an error (eg. malloc fails)
// Not all drawers will require this - if your drawer requires no initialisation
//   then I recommend that you define an init function that simply returns true
bool  dc_vgs_init (BMF_Font* font)
{
    // You will (probably) need one of these
    arg = font ? (DrawcArgs*)font->args
               : (DrawcArgs*)((BMF_stateGet(NULL))->args) ;

    arg->on = true;
    arg->x  = 3;
    arg->y  = 3;
    arg->a  = 0x70;

    return true;
}

//------------------------------------------------------------------------------
// This configures the drawer
// The first (of many) parameters is a BMF_Font*
// ...if this is NULL, the settings will be applied to the system-level drawer
// You will need to change the "vgs" to a nice ID for your drawer
// You will probably need to edit the argument list
// It may return false if there is an error (eg. malloc fails)
bool  dc_vgs_cfg (BMF_Font* font, const int x, const int y, const int a)
{
    // You will (probably) need one of these
    arg = font ? (DrawcArgs*)font->args
               : (DrawcArgs*)((BMF_stateGet(NULL))->args) ;

    arg->x = x;
    arg->y = y;
    arg->a = a;

    return true;
}

//------------------------------------------------------------------------------
// This drawer has a separate "shadow on/off" configurator
// The first (of many) parameters is a BMF_Font*
// ...if this is NULL, the settings will be applied to the system-level drawer
void  dc_vgs_shadow (BMF_Font* font, bool b)
{
    // You will (probably) need one of these
    arg = font ? (DrawcArgs*)font->args
               : (DrawcArgs*)((BMF_stateGet(NULL))->args) ;

    arg->on = b;
}

//------------------------------------------------------------------------------
// Every drawer MUST have one of these
// Draws a character to video with a(n optional) shadow

extern  GRRLIB_drawSettings  GRRLIB_Settings;
extern  Mtx                  GXmodelView2D;

FN_HOOK_DRAWC(drawc_vid_grr_shad)
{
    GXTexObj  texObj;
    Mtx       m, m1, m2, mv;

    BMF_State*  pBMFs;          // Pointer to the State Table
    f32         scr_l,  scr_r,  scr_t,  scr_b;
    f32         chr_l,  chr_r,  chr_t,  chr_b;

    guVector  axis = (guVector){0, 0, 1};

    // You will (probably) need one of these
    arg = (DrawcArgs*)args;

    GX_InitTexObj(&texObj, font->tex->data, font->tex->w, font->tex->h,
                  GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);

    if (GRRLIB_Settings.antialias == false)
        GX_InitTexObjLOD(&texObj, GX_NEAR, GX_NEAR,
                         0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

    GX_LoadTexObj(&texObj,      GX_TEXMAP0);
    GX_SetTevOp  (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc(GX_VA_TEX0,   GX_DIRECT);

    // Scaling & Rotation calculations
    guMtxIdentity  (m1);
    guMtxScaleApply(m1, m1, 1.0f, 1.0f, 1.0f);
    guMtxRotAxisDeg(m2, &axis, 0.0f);
    guMtxConcat    (m2, m1, m);
    guMtxConcat    (GXmodelView2D, m, mv);

    //! we need to set the scissor here to match the text area

    // Character specific offsets
    scr_t = y +chr->yoff;
    scr_l = x +chr->xoff;
    scr_b = y +chr->yoff +chr->h;
    scr_r = x +chr->xoff +chr->w;

    chr_t = chr->idx[BMF_TL].y;
    chr_l = chr->idx[BMF_TL].x;
    chr_b = chr->idx[BMF_BR].y;
    chr_r = chr->idx[BMF_BR].x;

    // We will need aceess to the State Table
    pBMFs = BMF_stateGet(NULL);

    // Draw shadow if required
    if (arg->on) {
        f32  scr_ls, scr_rs, scr_ts, scr_bs;
        f32  chr_ls, chr_rs, chr_ts, chr_bs;

        // Character specific offsets for the shadow
        scr_ts = scr_t +arg->y;
        scr_ls = scr_l +arg->x;
        scr_bs = scr_b +arg->y;
        scr_rs = scr_r +arg->x;

        chr_ts = chr_t;
        chr_ls = chr_l;
        chr_bs = chr_b;
        chr_rs = chr_r;

        // Deal with half-characters
        if (pBMFs->t > scr_ts) {
            chr_ts += font->pixh *(pBMFs->t - scr_ts);
            scr_ts  = pBMFs->t;
        }
        if (pBMFs->l > scr_ls) {
            chr_ls += font->pixw *(pBMFs->l - scr_ls);
            scr_ls  = pBMFs->l;
        }
        if (pBMFs->b < scr_bs) {
            chr_bs -= font->pixh *(scr_bs - pBMFs->b);
            scr_bs  = pBMFs->b;
        }
        if (pBMFs->r < scr_rs) {
            chr_rs -= font->pixw *(scr_rs - pBMFs->r);
            scr_rs  = pBMFs->r;
        }

        // Draw the shadow
        GX_LoadPosMtxImm(mv, GX_PNMTX0);
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
            GX_Position3f32(scr_ls, scr_ts, 0);
            GX_Color1u32   (arg->a);
            GX_TexCoord2f32(chr_ls, chr_ts);

            GX_Position3f32(scr_rs, scr_ts, 0);
            GX_Color1u32   (arg->a);
            GX_TexCoord2f32(chr_rs, chr_ts);

            GX_Position3f32(scr_rs, scr_bs, 0);
            GX_Color1u32   (arg->a);
            GX_TexCoord2f32(chr_rs, chr_bs);

            GX_Position3f32(scr_ls, scr_bs, 0);
            GX_Color1u32   (arg->a);
            GX_TexCoord2f32(chr_ls, chr_bs);
        GX_End();
    }

    // Deal with half-characters
    if (pBMFs->t > scr_t) {
        chr_t += font->pixh *(pBMFs->t - scr_t);
        scr_t  = pBMFs->t;
    }
    if (pBMFs->l > scr_l) {
        chr_l += font->pixw *(pBMFs->l - scr_l);
        scr_l  = pBMFs->l;
    }
    if (pBMFs->b < scr_b) {
        chr_b -= font->pixh *(scr_b - pBMFs->b);
        scr_b  = pBMFs->b;
    }
    if (pBMFs->r < scr_r) {
        chr_r -= font->pixw *(scr_r - pBMFs->r);
        scr_r  = pBMFs->r;
    }

    // Display the character
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(scr_l, scr_t, 0);
        GX_Color1u32   (0xFFFFFFFF);
        GX_TexCoord2f32(chr_l, chr_t);

        GX_Position3f32(scr_r, scr_t, 0);
        GX_Color1u32   (0xFFFFFFFF);
        GX_TexCoord2f32(chr_r, chr_t);

        GX_Position3f32(scr_r, scr_b, 0);
        GX_Color1u32   (0xFFFFFFFF);
        GX_TexCoord2f32(chr_r, chr_b);

        GX_Position3f32(scr_l, scr_b, 0);
        GX_Color1u32   (0xFFFFFFFF);
        GX_TexCoord2f32(chr_l, chr_b);
    GX_End();

    GX_LoadPosMtxImm(GXmodelView2D, GX_PNMTX0);
    GX_SetTevOp     (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc   (GX_VA_TEX0,   GX_NONE);

    return true;
}
