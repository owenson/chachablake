#include <stdio.h>
#include <string.h>
#include <blake2.h>
#include <ecrypt-sync.h>

void printHex(uint8_t *buf, uint16_t sz) {
    for (int i = 0; i < sz; i++) {
        printf("%02x ", buf[i]);
        if(i%16 == 15)
            puts("");
        else if(i%16 == 7)
            printf(" ");
    }
    puts("\n");
}

int main(int argc, const char *argv[])
{
//    puts("BLAKE TEST VECTOR\n");
//    char str[]="The quick brown fox jumps over the lazy dog";
//    uint8_t out[512/8];
//    blake2b(out, str,NULL, 512/8, strlen(str), 0);
//    printHex(out, sizeof(out));

    puts("\nCHACHA TEST VECTOR\n");
    ECRYPT_init();
    ECRYPT_ctx ctx;
    char msg[] = "Hello I'm a lumberjack";
    u8 key[16]={0x7d, 0x6d, 0x3d, 0x4b, 0xa6, 0xa9, 0x53, 0x11, 0x45, 0x48, 0x60, 0x4f, 0x5a, 0x9c, 0xf4, 0x04};
    u8 iv[8];
    //FILE *rnd = fopen("/dev/urandom", "rb");
    //if(!rnd) return 1;
    //fread(iv, 8, 1, rnd);
    //fread(key, 16, 1, rnd);
    //printHex(iv,8);
    memset(iv,0,8);
    memset(key,0,16);
    ECRYPT_keysetup(&ctx, key, 128, 0);
    ECRYPT_ivsetup(&ctx, iv);
    u8 buf[256];

    ECRYPT_keystream_bytes(&ctx, buf, 256);
    printHex(buf, 256);
    //ECRYPT_encrypt_bytes(&ctx, msg, buf, strlen(msg));
    //printHex(buf,strlen(msg));
    ////--decrypt
    //ECRYPT_ivsetup(&ctx, iv); // iv must be reset/synced with receiver
    //ECRYPT_decrypt_bytes(&ctx, buf, msg, strlen(msg));
    //puts(msg);

    return 0;
}
