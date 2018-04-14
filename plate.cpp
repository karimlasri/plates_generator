#include <iostream>
#include <time.h>
#include <unistd.h>


#define PLATE_NUMBER_SIZE 7

using namespace std;

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b; a=a-c; a=a^(c>>13);
    b=b-c; b=b-a; b=b^(a<<8);
    c=c-a; c=c-b; c=c^(b>>13);
    a=a-b; a=a-c; a=a^(c>>12);
    b=b-c; b=b-a; b=b^(a<<16);
    c=c-a; c=c-b; c=c^(b>>5);
    a=a-b; a=a-c; a=a^(c>>3);
    b=b-c; b=b-a; b=b^(a<<10);
    c=c-a; c=c-b; c=c^(b>>15);
    return(c%1000000);
}

void generate_random_number(char *number) {
    unsigned long seed = mix(clock(), time(NULL), getpid());
    srand(seed);
    for (int i = 0; i < PLATE_NUMBER_SIZE; i++) {
        char c;
        int index = rand() % 36;
        if (index < 26) {
            c = index + 'A';
        } else {
            c = index - 26 + '0';
        }
        number[i] = c;
    }
}

string number_to_string(const char *number) {
    char left[3] = {number[0], number[1], '\0'};
    char middle[4] = {number[2], number[3], number[4], '\0'};
    char right[3] = {number[5], number[6], '\0'};
    return string(left)+string(middle)+string(right);
}

bool string_to_number(const string& chars, char *number) {
    if (chars.size() < PLATE_NUMBER_SIZE) return false;
    for (int i = 0; i < PLATE_NUMBER_SIZE; i++) {
        number[i] = chars.at(i+chars.size()-PLATE_NUMBER_SIZE);
    }
    number[PLATE_NUMBER_SIZE] = '\0';
    return true;
}
