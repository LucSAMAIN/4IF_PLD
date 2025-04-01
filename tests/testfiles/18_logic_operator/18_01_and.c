int main() {
    int c = 0;

    if (1 && 1) {
        c = c+1;
    }
    if (1 && 0) {
        c = c+10;
    }
    if (0 && 1) {
        c = c+100;
    }

    return c;
}