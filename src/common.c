#include "common.h"

// 辅助函数：去除引号和换行
void clean_token(char *str) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src != '"' && *src != '\n' && *src != '\r') *dst++ = *src;
        src++;
    }
    *dst = '\0';
}

// 分组
int get_grp(int age) {
    if (age < 30) return 1;
    if (age <= 50) return 2;
    if (age > 50) return 3;
    return 0;
}

// 加载数据
int load_samples(const char *fname, Sample **list_out) {
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        printf("Error: Metadata file '%s' missing.\n", fname);
        return -1;
    }

    char line[256];
    int n = 0;

    // 1. 第一次扫描：计算行数
    if (fgets(line, sizeof(line), fp) == NULL) { 
        fclose(fp); 
        return 0; 
    }
    while (fgets(line, sizeof(line), fp)) n++;

    Sample *list = (Sample *)malloc(n * sizeof(Sample));
    if (!list) { 
        fclose(fp); 
        return -1; 
    }

    rewind(fp);
    
    if (fgets(line, sizeof(line), fp) == NULL) {
        free(list);
        fclose(fp);
        return -1;
    }

    // 2.第二次扫描：存储数据
    int i = 0;
    while (i < n && fscanf(fp, "%s %d", list[i].id, &list[i].age) != EOF) {
        clean_token(list[i].id);
        list[i].grp = get_grp(list[i].age);
        list[i].total_counts = 0; // 初始化
        i++;
    }
    fclose(fp);
    
    *list_out = list; // 传值 
    return n;
}
