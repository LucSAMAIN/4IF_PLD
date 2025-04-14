void print_int(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x / 10 != 0) {
        print_int(x / 10);
    }
    putchar(x % 10 + '0');
}

int fast_pow(int base, int exp) {
    if (exp == 0) {
        return 1;
    }
    if (exp == 1) {
        return base;
    }

    if (exp % 2 == 0) {
        return fast_pow(base*base, exp / 2);
    }
    else {
        return base*fast_pow(base*base, exp / 2);
    }
}

void print_intro() {
    putchar('E');
    putchar('n');
    putchar('t');
    putchar('e');
    putchar('r');
    putchar(' ');
    putchar('b');
    putchar('a');
    putchar('s');
    putchar('e');
    putchar(' ');
    putchar('a');
    putchar('n');
    putchar('d');
    putchar(' ');
    putchar('e');
    putchar('x');
    putchar('p');
    putchar(' ');
    putchar(':');
    putchar('\n');
}

int main() {
    int c, base=0, exp=0, res=0;
    print_intro();
    while ((c = getchar()) != '\n') {
        base *= 10;
        base += c-'0';
    }
    while ((c = getchar()) != '\n') {
        exp *= 10;
        exp += c-'0';
    }
    res = fast_pow(base, exp);

    print_int(base);
    putchar('^');
    print_int(exp);
    putchar('=');
    print_int(res);
    putchar('\n');

    return 0;
}