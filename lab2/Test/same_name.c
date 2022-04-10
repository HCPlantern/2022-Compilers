// double a = 1.0; // conflict
int a() {
    return 0;
}

// int a = 1;  // conflict with the func a.

int main() {
    int a = 1;  // does not conflict with the func a.
}