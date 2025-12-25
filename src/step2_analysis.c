#include "common.h"

#define MAX_LINE 200000 
#define MAX_SAMPLES 500 // 用于 map 数组的大小

// --- 标程 (Standard Code) START ---
double calculate_p_value(double t_value, int df) {
    double p = 0.0;
    if (df <= 0) return 1.0;
    double x = df / (df + t_value * t_value);
    double a = 0.5 * df;
    double b = 0.5;
    p = 2 * (1 - 0.5 * pow(1 - x, a) * pow(x, b));
    if (p < 0) p = 0; if (p > 1) p = 1;
    return p;
}

double welch_t_test(double *group1, int n1, double *group2, int n2, double *t_value) {
    if (n1 <= 1 || n2 <= 1) return -1;
    double sum1 = 0.0, sum_sq1 = 0.0;
    for (int i = 0; i < n1; i++) { sum1 += group1[i]; sum_sq1 += group1[i] * group1[i]; }
    double mean1 = sum1 / n1;
    double var1 = (sum_sq1 - sum1 * sum1 / n1) / (n1 - 1);
    
    double sum2 = 0.0, sum_sq2 = 0.0;
    for (int i = 0; i < n2; i++) { sum2 += group2[i]; sum_sq2 += group2[i] * group2[i]; }
    double mean2 = sum2 / n2;
    double var2 = (sum_sq2 - sum2 * sum2 / n2) / (n2 - 1);
    
    double se = sqrt(var1 / n1 + var2 / n2);
    if (se == 0.0) { *t_value = 0; return (n1 < n2 ? n1 : n2) - 1; }
    *t_value = (mean1 - mean2) / se;
    
    double numerator = pow(var1 / n1 + var2 / n2, 2);
    double denominator = pow(var1 / n1, 2) / (n1 - 1) + pow(var2 / n2, 2) / (n2 - 1);
    return numerator / denominator;
}
// --- 标程 END ---

int find_idx_by_id(Sample *list, int n, char *id) {
    for (int i = 0; i < n; i++) {
        if (strcmp(list[i].id, id) == 0) return i;
    }
    return -1; 
}

int main() {
    // 1. 调用公共模块加载样本
    Sample *samples = NULL;
    int n_samples = load_samples("data/GSE277909_ID_Age.txt", &samples);
    printf("Step 2 (Modular): Loaded %d samples.\n", n_samples);

    FILE *fp_in = fopen("data/GSE277909_genecounts_SLE_bulk.csv", "r");
    if (!fp_in) { printf("Error: CSV missing.\n"); return 1; }

    char line[MAX_LINE];
    int col_map[MAX_SAMPLES]; 
    int col_count = 0;

    // Pass 1: Library Sizes
    printf("Pass 1: Calculating Library Sizes...\n");
    if (fgets(line, sizeof(line), fp_in)) {
        char *token = strtok(line, ","); 
        token = strtok(NULL, ","); 
        while (token != NULL) {
            clean_token(token);
            col_map[col_count] = find_idx_by_id(samples, n_samples, token);
            col_count++;
            token = strtok(NULL, ",");
        }
    }

    while (fgets(line, sizeof(line), fp_in)) {
        char *token = strtok(line, ","); 
        int col_idx = 0;
        token = strtok(NULL, ",");
        while (token != NULL && col_idx < col_count) {
            int s_idx = col_map[col_idx];
            if (s_idx != -1) {
                clean_token(token);
                samples[s_idx].total_counts += atof(token);
            }
            token = strtok(NULL, ",");
            col_idx++;
        }
    }
    
    // Pass 2: T-test
    printf("Pass 2: Applying Welch's T-Test...\n");
    rewind(fp_in); 
    if(fgets(line, sizeof(line), fp_in) == NULL) return 1;

    FILE *fp_out = fopen("result/t_test_results.txt", "w");
    fprintf(fp_out, "GeneID\tT_G1_vs_G2\tT_G2_vs_G3\tT_G1_vs_G3\n");

    int gene_idx = 0;
    double g1_data[MAX_SAMPLES], g2_data[MAX_SAMPLES], g3_data[MAX_SAMPLES];

    while (fgets(line, sizeof(line), fp_in)) {
        int n1 = 0, n2 = 0, n3 = 0;
        char *token = strtok(line, ",");
        clean_token(token);
        char *gene_id = token; 
        
        int col_idx = 0;
        token = strtok(NULL, ",");
        
        while (token != NULL && col_idx < col_count) {
            int s_idx = col_map[col_idx];
            if (s_idx != -1) {
                int grp = samples[s_idx].grp;
                clean_token(token);
                double raw_count = atof(token);
                double lib_size = samples[s_idx].total_counts;
                
                if (lib_size > 0) {
                    double cpm = (raw_count / lib_size) * 1000000.0;
                    double val = log2(cpm + 1.0);
                    if (grp == 1) g1_data[n1++] = val;
                    else if (grp == 2) g2_data[n2++] = val;
                    else if (grp == 3) g3_data[n3++] = val;
                }
            }
            token = strtok(NULL, ",");
            col_idx++;
        }

        double t12 = 0, t23 = 0, t13 = 0;
        welch_t_test(g1_data, n1, g2_data, n2, &t12);
        welch_t_test(g2_data, n2, g3_data, n3, &t23);
        welch_t_test(g1_data, n1, g3_data, n3, &t13);

        fprintf(fp_out, "%s\t%.4f\t%.4f\t%.4f\n", gene_id, t12, t23, t13);
        gene_idx++;
        if (gene_idx % 2000 == 0) printf("Processed %d genes...\r", gene_idx);
    }
    printf("\nDone. Processed %d genes.\n", gene_idx);
    
    free(samples); // 记得释放内存
    fclose(fp_in);
    fclose(fp_out);
    return 0;
}
