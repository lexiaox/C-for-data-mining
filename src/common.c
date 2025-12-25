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

// 分组逻辑
int get_grp(int age) {
    if (age < 30) return 1;
    if (age <= 50) return 2;
    if (age > 50) return 3;
    return 0;
}

// 核心：加载样本函数
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

    // 2. 分配内存
    Sample *list = (Sample *)malloc(n * sizeof(Sample));
    if (!list) { 
        fclose(fp); 
        return -1; 
    }

    // 3. 第二次扫描：读取数据
    rewind(fp);
    
    // [注意这里]：必须检查 rewind 后的 fgets 返回值
    if (fgets(line, sizeof(line), fp) == NULL) {
        free(list);
        fclose(fp);
        return -1;
    }

    int i = 0;
    while (i < n && fscanf(fp, "%s %d", list[i].id, &list[i].age) != EOF) {
        clean_token(list[i].id);
        list[i].grp = get_grp(list[i].age);
        list[i].total_counts = 0; // 初始化
        i++;
    }
    fclose(fp);
    
    *list_out = list; // 把数组指针传出去
    return n;
}
