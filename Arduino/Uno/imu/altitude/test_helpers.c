#include "test_helpers.h"

static int test_vector = 0;
static int check_number = 0;

void increment_test_vector (void) {
    test_vector++;
    check_number = 0;
}

bool check(bool condition) {
    bool ret;
    if (condition) {
        fprintf(stdout, "Vector %d Check %d Passed\n", test_vector, check_number);        
        ret = true;
    } else {
        fprintf(stdout, "Vector %d Check %d Failed\n", test_vector, check_number);                
        ret = false;
    }
    check_number++;
    return ret;
}
