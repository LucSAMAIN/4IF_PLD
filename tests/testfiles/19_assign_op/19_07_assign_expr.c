int main() {
    int a = 24;
    a *= (a += 6*(a+=4));
    return a;
}