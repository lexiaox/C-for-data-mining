#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_GENES 20000 
#define TOP_N 50        

typedef struct {
    char id[32];
    double t[4]; // t[1]=G1vG2, t[2]=G2vG3, t[3]=G1vG3
} GeneData;

GeneData *all_genes; // Global

// 比较函数：按绝对值降序
int cmp_t1(const void *a, const void *b) {
    double v1 = fabs(all_genes[*(int*)a].t[1]);
    double v2 = fabs(all_genes[*(int*)b].t[1]);
    return (v2 > v1) - (v2 < v1); 
}
int cmp_t2(const void *a, const void *b) {
    double v1 = fabs(all_genes[*(int*)a].t[2]);
    double v2 = fabs(all_genes[*(int*)b].t[2]);
    return (v2 > v1) - (v2 < v1);
}
int cmp_t3(const void *a, const void *b) {
    double v1 = fabs(all_genes[*(int*)a].t[3]);
    double v2 = fabs(all_genes[*(int*)b].t[3]);
    return (v2 > v1) - (v2 < v1);
}

// 辅助函数：打印交集
void print_intersection(int *list1, int *list2, const char *label) {
    printf("\n=== Checking Intersection: %s ===\n", label);
    int found = 0;
    for (int i = 0; i < TOP_N; i++) {
        for (int j = 0; j < TOP_N; j++) {
            if (strcmp(all_genes[list1[i]].id, all_genes[list2[j]].id) == 0) {
                printf("  [MATCH] %s\n", all_genes[list1[i]].id);
                found++;
            }
        }
    }
    if (found == 0) printf("  Result: None.\n");
    else printf("  Total: %d common genes.\n", found);
}

int main() {
    FILE *fp = fopen("result/t_test_results.txt", "r");
    if (!fp) { printf("Error: Run step2 first.\n"); return 1; }

    all_genes = (GeneData *)malloc(MAX_GENES * sizeof(GeneData));
    char line[256];
    int n = 0;

    if (fgets(line, sizeof(line), fp) == NULL) return 1; // Skip Header

    while (fscanf(fp, "%s %lf %lf %lf", 
           all_genes[n].id, &all_genes[n].t[1], &all_genes[n].t[2], &all_genes[n].t[3]) != EOF) {
        n++;
    }
    printf("Step 3: Loaded %d genes.\n", n);
    fclose(fp);

    // 创建三个索引数组
    int *idx1 = (int*)malloc(n*sizeof(int)); // G1 vs G2
    int *idx2 = (int*)malloc(n*sizeof(int)); // G2 vs G3
    int *idx3 = (int*)malloc(n*sizeof(int)); // G1 vs G3
    
    for(int i=0; i<n; i++) idx1[i] = idx2[i] = idx3[i] = i;

    // 分别排序
    qsort(idx1, n, sizeof(int), cmp_t1);
    qsort(idx2, n, sizeof(int), cmp_t2);
    qsort(idx3, n, sizeof(int), cmp_t3);

    // 打印 Top 5 (G1 vs G3) 看看最显著的
    printf("\n--- Top 5 (G1 vs G3: Young vs Old) ---\n");
    for(int i=0; i<5; i++) printf("%s (T=%.2f)\n", all_genes[idx3[i]].id, all_genes[idx3[i]].t[3]);

    // 核心：全方位找交集
    print_intersection(idx1, idx2, "G1vG2 (Young-Mid) AND G2vG3 (Mid-Old)");
    print_intersection(idx1, idx3, "G1vG2 (Young-Mid) AND G1vG3 (Young-Old)");
    print_intersection(idx2, idx3, "G2vG3 (Mid-Old)   AND G1vG3 (Young-Old)");

    FILE *fp_out = fopen("result/final_result.txt", "w");

    // 三者交集
    printf("\n=== Checking Intersection: ALL 3 LISTS ===\n");
    fprintf(fp_out,"\n=== Checking Intersection: ALL 3 LISTS ===\n");
    int found_all = 0;
    for (int i = 0; i < TOP_N; i++) {
        char *id = all_genes[idx1[i]].id;
        int in2 = 0, in3 = 0;
        
        for(int j=0; j<TOP_N; j++) if(strcmp(id, all_genes[idx2[j]].id)==0) in2=1;
        for(int k=0; k<TOP_N; k++) if(strcmp(id, all_genes[idx3[k]].id)==0) in3=1;
        
        if (in2 && in3) {
            printf("  [SUPER MATCH] %s\n", id);
            fprintf(fp_out,"  [SUPER MATCH] %s\n", id);
            found_all++;
        }
    }
    if(found_all == 0) printf("  Result: None.\n");
    if(found_all == 0) fprintf(fp_out,"  Result: None.\n");
    free(all_genes); free(idx1); free(idx2); free(idx3);
    return 0;
}
