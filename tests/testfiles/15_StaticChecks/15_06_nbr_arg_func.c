int func(int a) {
    return a;
}

int main() {
    int a = func(1, 2);
    int b = func();
    int c = func(1);

    return c;
}