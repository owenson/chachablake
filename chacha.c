/*
chacha-ref.c version 20080118
D. J. Bernstein
Public domain.
*/

#include <string.h>
#include <stdio.h>
#include "ecrypt-sync.h"

#define ROTATE(v,c) (ROTL32(v,c))
#define XOR(v,w) ((v) ^ (w))
#define PLUS(v,w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v),1))

#define QUARTERROUND(a,b,c,d) \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);

#define ROUNDS 8

u8 keystream[64];
u8 keystream_idx = 64;

static void salsa20_wordtobyte(u8 output[64],const u32 input[16])
{
  u32 x[16];
  int i;

  for (i = 0;i < 16;++i) x[i] = input[i];
  for (i = ROUNDS;i > 0;i -= 2) {
    QUARTERROUND( 0, 4, 8,12)
    QUARTERROUND( 1, 5, 9,13)
    QUARTERROUND( 2, 6,10,14)
    QUARTERROUND( 3, 7,11,15)
    QUARTERROUND( 0, 5,10,15)
    QUARTERROUND( 1, 6,11,12)
    QUARTERROUND( 2, 7, 8,13)
    QUARTERROUND( 3, 4, 9,14)
  }
  for (i = 0;i < 16;++i) x[i] = PLUS(x[i],input[i]);
  for (i = 0;i < 16;++i) U32TO8_LITTLE(output + 4 * i,x[i]);
}

static const char sigma[16] = "expand 32-byte k";
static const char tau[16] = "expand 16-byte k";

void chacha_init(CHACHA_ctx *x,const u8 *k,u32 kbits,u32 *ivbits)
{
  const char *constants;

  x->input[4] = U8TO32_LITTLE(k + 0);
  x->input[5] = U8TO32_LITTLE(k + 4);
  x->input[6] = U8TO32_LITTLE(k + 8);
  x->input[7] = U8TO32_LITTLE(k + 12);
  if (kbits == 256) { /* recommended */
    k += 16;
    constants = sigma;
  } else { /* kbits == 128 */
    constants = tau;
  }
  x->input[8] = U8TO32_LITTLE(k + 0);
  x->input[9] = U8TO32_LITTLE(k + 4);
  x->input[10] = U8TO32_LITTLE(k + 8);
  x->input[11] = U8TO32_LITTLE(k + 12);
  x->input[0] = U8TO32_LITTLE(constants + 0);
  x->input[1] = U8TO32_LITTLE(constants + 4);
  x->input[2] = U8TO32_LITTLE(constants + 8);
  x->input[3] = U8TO32_LITTLE(constants + 12);

  if(ivbits != NULL)
      chacha_ivsetup(x, ivbits);

  keystream_idx = 64;
}

void chacha_ivsetup(CHACHA_ctx *x,const u8 *iv)
{
  x->input[12] = 0;
  x->input[13] = 0;
  x->input[14] = U8TO32_LITTLE(iv + 0);
  x->input[15] = U8TO32_LITTLE(iv + 4);
  keystream_idx = 64;
}

// rewritten to keep in sync - no need to make requests that are
// multiple of 64 bytes to keep in sync, just call at will
// reinit key or iv using above funcs to reset stream
// maximum is 2^32 bytes per call
/* stopping at 2^70 bytes per nonce is user's responsibility */
// remember: stream cipher .. encrypt = decrypt, symmetrical.
void chacha_encryptdecrypt(CHACHA_ctx *x,const u8 *m,u8 *c,u32 bytes)
{
  u32 i=0;

  if (!bytes) return;
  while(i<bytes) {
    // need more keystream bytes?
    if(keystream_idx == 64) {
        salsa20_wordtobyte(keystream,x->input);
        // advance IV
        x->input[12] = PLUSONE(x->input[12]);
        if (!x->input[12]) x->input[13] = PLUSONE(x->input[13]);
        keystream_idx = 0;
    }

    for(;i<bytes && keystream_idx < 64; i++) 
        c[i] = m[i] ^ keystream[keystream_idx++];
  }
}

void chacha_keystream(CHACHA_ctx *x,u8 *stream,u32 bytes)
{
  u32 i;
  for (i = 0;i < bytes;++i) stream[i] = 0;
  chacha_encryptdecrypt(x,stream,stream,bytes);
}

