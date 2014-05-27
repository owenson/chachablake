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
    puts("BLAKE TEST VECTOR\n");
    char str[]="The quick brown fox jumps over the lazy dog";
    uint8_t out[512/8];
    blake2b(out, str,NULL, 512/8, strlen(str), 0);
    printHex(out, sizeof(out));

    puts("\nCHACHA TEST VECTOR\n");
    ECRYPT_init();
    ECRYPT_ctx ctx;
    u8 key[16];
    memset(key, 0, 16);

    ECRYPT_keysetup(&ctx, key, 128, 0);
    // key is zero so use for iv too - don't do this in real impl
    ECRYPT_ivsetup(&ctx, key);
    u8 buf[128];
    ECRYPT_keystream_bytes(&ctx, buf, 128);

    printHex(buf, 128);

    return 0;
}
