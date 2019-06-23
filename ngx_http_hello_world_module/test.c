#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
typedef struct {
    size_t      len;
	u_char     *data;
} ngx_str_t;
#define strConcat(des, src)\
    {u_char * temp = NULL;\
    size_t i, size = des.len + src.len;\
    temp = (u_char*)calloc(size, sizeof(u_char));\
    for(i = 0; i < des.len; i++){\
     temp[i] = des.data[i];\
    }\
    for(i = des.len; i < size; i++){\
     temp[i] = src.data[i-des.len];\
    }\
    free(des.data);\
    des.data = temp;\
    des.len = size;}

int main(){
	
}