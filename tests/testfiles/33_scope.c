int main() {
    int i = 0;
    i = i;
    {
        int a = i;
    }
    {
        int i = 1;
        int a = i;
    }
    {
        int a = i;
    }
    return i;
}