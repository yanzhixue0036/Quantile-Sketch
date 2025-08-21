#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "tools.h"

/*
    Compare two doubles
*/
int double_cmp(const void * a, const void * b)
{
	double first = *(double *)a;
	double second = *(double *)b;

	if (fabs(first - second) < double_EPSILON)
		return 0;
	else if (first < second)
		return -1;
	else
		return 1;
}

/*
    Extract the sign bit of a double.
    return 0 if positive else 1.
*/
int extract_sign_bit_of_double(double value) {
    uint64_t *p = (uint64_t *)&value;
    return (*p >> 63) & 1;

    // uint64_t bits;
    // memcpy(&bits, &value, sizeof(bits));
    // return (bits >> 63) & 1;
}

/*
    Write results.
*/
void create_directory_if_not_exists(const char *dir) {
    struct stat st = {0};

    if (stat(dir, &st) == -1) {
        if (mkdir(dir, 0700) != 0) {
            printf("Failed to create directory %s\n", dir);
            exit(EXIT_FAILURE);
        }
    }
}

void write_to_csv(const char *filename, double *row1, double *row2, size_t len)
{
	create_directory_if_not_exists("result");
	char filepath[256];
    snprintf(filepath, sizeof(filepath), "result/%s", filename);

    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        printf("Failed to open file for writing\n");
        return;
    }

    fprintf(fp, "Quantile, Req_Error\n");
    for (size_t i = 0; i < len; i++) {
        fprintf(fp, "%f,%f\n", row1[i], row2[i]);
    }
    fclose(fp);

    printf("Results have been written to %s\n", filepath);
}
