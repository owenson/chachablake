#include <stdio.h>
#include <string.h>
#include <blake2.h>
#include <ecrypt-sync.h>

int main(int argc, const char *argv[])
{
    puts("BLAKE TEST VECTOR\n");
    char str[]="The quick brown fox jumps over the lazy dog";
    uint8_t out[512/8];
    blake2b(out, str,NULL, 512/8, strlen(str), 0);
    for (int i = 0; i < sizeof(out); i++) {
        printf("%02x ", out[i]);
    }

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

    for (int i = 0; i < 128; i++) {
        printf("%02x ", buf[i]);
    }
    return 0;
}
