#include <stdlib.h>
#include <stdint.h>
int main()
{
    uint8_t *a = malloc(2);
    uint8_t* b = a + 1;
    free(b);
    return 0;
}
