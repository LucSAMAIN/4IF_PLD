#include <stdio.h>

int main() {
    int x = 0;
    while (x < 5) {
        x = x+1;
        if (x == 3) {
            x = x+10;
        }
    }
    return x;
}   