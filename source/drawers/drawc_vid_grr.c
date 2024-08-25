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

extern  GRRLIB_drawSettings  GRRLIB_Settings;
extern  Mtx                  GXmodelView2D;

// This is the a most basic drawer
// It is designed for use with GRRLIB
// It has no configurable options
// It simply cuts the letter from the texture
//           & draws it on the screen at the relevant coordinates
//
// For information on overriding this default
//   search bmf.h for BMF_DRAWC_DFLT in and read the comment just above it
FN_HOOK_DRAWC(drawc_vid_grr)
{
    GXTexObj    texObj;         // Texture information
    Mtx         m, m1, m2, mv;  // Scaling & Rotation Matrices

    BMF_State*  pBMFs;          // Pointer to the State Table
    f32         scr_l, scr_r, scr_t, scr_b;
    f32         chr_l, chr_r, chr_t, chr_b;

    guVector  axis = (guVector){0, 0, 1};

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
