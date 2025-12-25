#include "common.h"

int main() {
    Sample *samples = NULL;
    int n = load_samples("data/GSE277909_ID_Age.txt", &samples);

    if (n <= 0) return 1;

    int c1=0, c2=0, c3=0;
    for(int i=0; i<n; i++) {
        if (samples[i].grp == 1) c1++;
        else if (samples[i].grp == 2) c2++;
        else if (samples[i].grp == 3) c3++;
    }

    printf("--- Grouping Report (Powered by Common Module) ---\n");
    printf("Group 1 (<30):   %d\n", c1);
    printf("Group 2 [30-50]: %d\n", c2);
    printf("Group 3 (>50):   %d\n", c3);
    
    free(samples);
    return 0;
}
