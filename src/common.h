#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 统一的样本结构体
typedef struct {
    char id[32];
    int age;
    int grp;             // 1=Group1, 2=Group2, 3=Group3
    double total_counts; // Step 2 用的文库大小
} Sample;

// 函数声明
int get_grp(int age);
int load_samples(const char *fname, Sample **list_out); // 注意这里用二级指针
void clean_token(char *str);
#endif
