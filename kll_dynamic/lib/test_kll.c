#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "tools.h"
#include "utest.h"
#include <kll_sketch.h>



UTEST(median, simple_on_million)
{
    KLLSketch *s = kll_sketch_new(1000);

    for (size_t i = 0; i < 1000000; i++)
        kll_sketch_add(s, i);

    //kll_sketch_print(s);
    KLLQuantiles q = kll_sketch_get_quantiles(s);
    double median = kll_sketch_quantiles_query(q, 0.5);
    printf("%f\n", median);
    ASSERT_TRUE(fabs(500000.0 - median) < 1500000);
    kll_sketch_free(s);
}

UTEST(Uniform_randomized, delete)
{
    size_t count = 999999;
    KLLSketch *s = kll_sketch_new(pow(2, 10));

    double *arr = (double *) malloc(sizeof(double) * count);

    srand(time(0));
    for (size_t i = 0; i < count; i++)
    {
        arr[i] = rand();
        kll_sketch_add(s, arr[i]);
    }
	
    double delete_ratio = 0.5;
    delete_random_elements(s, arr, &count, delete_ratio);
    kll_sketch_quantiles_test(s, arr, count);
    kll_sketch_free(s);
}

#define MAX_LINE_LENGTH (int)pow(10, 5)
UTEST(TPC_C, delete)
{   
    /* csv_filename[target_column]
       customer[10], [11]; item[3]; 
    */
    int target_column = 10;
    const char *csv_filename = "../reference/TPC-C/customer.csv";
    size_t count = 0;
    double *arr = NULL;

    FILE *file = fopen(csv_filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    rewind(file);

    // modify
    count = count / 100;

    arr = (double *)malloc(sizeof(double) * count);
    if (!arr) {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    size_t index = 0;
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        int column = 0;

        while (token && index<count) {
            if (column == target_column) {
                arr[index++] = atof(token);
                break;
            }
            token = strtok(NULL, ",");
            column++;
        }
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);

    int k = (int)pow(2, 9);
    KLLSketch *s = kll_sketch_new(k);
    for (size_t i = 0; i < count; i++) {
        kll_sketch_add(s, arr[i]);
    }

    double delete_ratio = 0.3;
    delete_random_elements(s, arr, &count, delete_ratio);
    
    kll_sketch_quantiles_test(s, arr, count);
    
    printf("File size %ld, totally add %ld items.\n", file_size, count);

    kll_sketch_free(s);
    free(arr);
}

UTEST_MAIN();
