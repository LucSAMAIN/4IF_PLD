int main() {
    int c = 0;

    if (1 && (c = 1)) {

    }
    if (0 && (c = 10)) {
        
    }

    int b = 1;
    if (c == 10 && (b = 5)) {

    }

    return b;
}