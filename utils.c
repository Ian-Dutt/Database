#include <utils.h>

int sum_ints(int arr[], int num_ints){
    int ret = 0;
    for(int i = 0; i < num_ints; ++i){
        ret+= arr[i];
    }
    return ret;
}