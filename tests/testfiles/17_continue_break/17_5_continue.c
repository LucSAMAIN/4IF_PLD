int main() {
    int i = 0;
    while (i < 10) {
        if (i % 2 == 1) {
            i = i+1;
            continue;
        }
        putchar(i+'0');
        i = i+1;
    }

    return 0;
}