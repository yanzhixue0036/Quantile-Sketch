#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "utest.h"
#include <kll_sketch.h>

static int double_cmp(const void * a, const void * b)
{
	double first = *(double *)a;
	double second = *(double *)b;

	if (fabs(first - second) < 0.0001)
		return 0;
	else if (first < second)
		return -1;
	else
		return 1;
}

UTEST(median, simple_on_million)
{
    KLLSketch *s = kll_sketch_new(1000);

    for (size_t i = 0; i < 1000000; i++)
        kll_sketch_update(s, i);

    //kll_sketch_print(s);
    KLLQuantiles q = kll_sketch_get_quantiles(s);
    double median = kll_sketch_quantiles_query(q, 0.5);
    printf("%f\n", median);
    ASSERT_TRUE(fabs(500000.0 - median) < 100);
    kll_sketch_free(s);
}

UTEST(median, randomized)
{
    size_t count = 999999;
    KLLSketch *s = kll_sketch_new(10000);

    double *arr = (double *) malloc(sizeof(double) * count);

    srand(time(0));
    for (size_t i = 0; i < count; i++)
    {
        arr[i] = rand();
        kll_sketch_update(s, arr[i]);
    }
	qsort(arr, count, sizeof(double), double_cmp);
    double actual = arr[500000];

    //kll_sketch_print(s);
    KLLQuantiles q = kll_sketch_get_quantiles(s);
    double median = kll_sketch_quantiles_query(q, 0.5);
    printf("%f - %f, diff=%f\n", actual, median, actual - median);
    ASSERT_TRUE(fabs(actual - median) < 150000);
    kll_sketch_free(s);
}

UTEST_MAIN();
