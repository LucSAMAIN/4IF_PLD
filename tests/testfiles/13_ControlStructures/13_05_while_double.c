#include <stdio.h>

int main() {
    double x = 19.78;
    while (x < 20.2) {
        putchar(x+'0');
        putchar('\n');
        x = x+0.1;
    }
    return 0;
}