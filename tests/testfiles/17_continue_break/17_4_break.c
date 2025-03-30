int main() {
    int i = 0;
    while (i < 5) {
        putchar(i+'a');
        if (i == 3) {
            break;
        }
        i = i+1;
    }

    return 0;
}