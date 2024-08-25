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

#ifndef _PEEKPOKE_H_
#define _PEEKPOKE_H_

#define PEEK_MAX      u32
#define PEEK(p)       ((PEEK_MAX)( *((u8*)(p)) ))
#define PEEK8(p)      ( PEEK(p) )

#define PEEK_LE(p)    ( PEEK(p) )
#define PEEK8_LE(p)   ( PEEK(p) )
#define PEEK16_LE(p)  ((u16)( PEEK8_LE (p) | (PEEK8_LE (((u8*)(p))+1) <<8 ) ))
#define PEEK32_LE(p)  ((u32)( PEEK16_LE(p) | (PEEK16_LE(((u8*)(p))+2) <<16) ))

#define PEEK_BE(p)    ( PEEK(p) )
#define PEEK8_BE(p)   ( PEEK(p) )
#define PEEK16_BE(p)  ((u16)( (PEEK8_BE (p) <<8 ) | PEEK8_BE (((u8*)(p))+1) ))
#define PEEK32_BE(p)  ((u32)( (PEEK16_BE(p) <<16) | PEEK16_BE(((u8*)(p))+2) ))

#endif //_PEEKPOKE_H_
