int global = 99;
int array[5];

int foo() {
    return 299;
}

int *bar() {
    return &global;
}

int bar_index(int a) {
    int sum = 0;
    for (int i = 0; i < a; i++) {
        sum += array[i];
    }

    return sum;
}

int bar2() {
    return global;
}
