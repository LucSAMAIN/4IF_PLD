int main() {
    int i = 0;
    int a = 4;
    {
        int a = 6;
        i = a;
    }
    return i;
}