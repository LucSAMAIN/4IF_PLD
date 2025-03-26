int func(int a) {
    return a;
}

void f() {

}

int main() {
    int a = func(1);
    int b = func(f());

    return b;
}