#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ROWS 10000
#define STATUS_LEN 32

char statuses[MAX_ROWS][STATUS_LEN];
double rr[MAX_ROWS];
int total_rows = 0;

int strcmp_custom(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

int read_csv(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[256];
    fgets(line, sizeof(line), fp); // skip header

    while (fgets(line, sizeof(line), fp)) {
        char *token;
        token = strtok(line, ","); // Time (ignore)
        token = strtok(NULL, ","); // Character_status

        if (token) {
            sscanf(token, "%31s", statuses[total_rows]);
        }

        token = strtok(NULL, ","); // rr
        if (token) {
            rr[total_rows] = atof(token);
        }

        total_rows++;
        if (total_rows >= MAX_ROWS) break;
    }

    fclose(fp);
    return 1;
}

void calculate_pnn50() {
    // หาว่ามีกี่ gesture type
    char unique[20][STATUS_LEN];
    int counts[20] = {0};
    double values[20][MAX_ROWS];
    int group_count = 0;

    for (int i = 0; i < total_rows; i++) {
        int found = 0;
        for (int j = 0; j < group_count; j++) {
            if (strcmp_custom(statuses[i], unique[j])) {
                values[j][counts[j]++] = rr[i];
                found = 1;
                break;
            }
        }
        if (!found) {
            sscanf(statuses[i], "%31s", unique[group_count]);
            values[group_count][0] = rr[i];
            counts[group_count] = 1;
            group_count++;
        }
    }

    // คำนวณ pNN50
    printf("Status\t\tpNN50(%%)\tRR_count\n");
    for (int i = 0; i < group_count; i++) {
        int nn50 = 0;
        int total = 0;
        for (int j = 1; j < counts[i]; j++) {
            double diff = fabs(values[i][j] - values[i][j - 1]);
            if (diff > 50.0) nn50++;
            total++;
        }

        double pnn50 = total > 0 ? ((double)nn50 / total) * 100.0 : 0.0;
        printf("%-15s\t%.2f\t\t%d\n", unique[i], pnn50, counts[i]);
    }
}

int main() {
    if (!read_csv("0000_man.csv")) {
        printf("Error: cannot read file\n");
        return 1;
    }

    calculate_pnn50();
    return 0;
}
