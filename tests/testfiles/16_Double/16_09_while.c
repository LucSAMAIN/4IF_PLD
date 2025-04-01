int main() {
    double x = 0;
    
    while (x != 1) {
        if (x < 0.5) {
            x = 0.5;
        }
        else {
            x = 1;
        }
    }

    return x;
}