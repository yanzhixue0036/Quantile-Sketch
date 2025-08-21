#include "build/FMSConfig.h"
#include "DDsketch/DDsketch.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <math.h>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

using namespace std;
using namespace chrono;

#define double_EPSILON 1e-6 // precision used for floating point comparisons

int double_cmp(const void *a, const void *b)
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
#define MAX_LINE_LENGTH (int)pow(10, 5)
int main(int argc, char *argv[])
{
	cout << "argc: " << argc << endl;
	cout << " Version " << FMS_VERSION_MAJOR << "."
		 << FMS_VERSION_MINOR << ": DPCOC under stream setting" << endl;
	unsigned repeat_parameter = 1, repeat_times = 1, num_elements = 100'000, seed;
	unsigned M = 1024;
	vector<double> quantile(100, 0);
	vector<double> errs_quantile(100, 0);
	double begin, end;

	// process TPC-C
	int target_column = 3;
	const char *csv_filename = "../../kll_dynamic/reference/TPC-C/customer.csv";
	size_t count = 0;
	double *arr = NULL;

	FILE *file = fopen(csv_filename, "r");
	if (!file)
	{
		perror("Failed to open file");
		return 0;
	}

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), file))
	{
		count++;
		// cout<<"count: "<<count<<endl;
	}
	cout<<"file line number: "<<count<<endl;
	return 0;
	rewind(file);

	// modify
	// count = count / 100;
	printf("totally add %ld items.\n", count);

	arr = (double *)malloc(sizeof(double) * count);
	if (!arr)
	{
		perror("Failed to allocate memory");
		fclose(file);
		return 0;
	}

	size_t index = 0;
	while (fgets(line, sizeof(line), file))
	{
		char *token = strtok(line, ",");
		int column = 0;

		while (token && index < count)
		{
			if (column == target_column)
			{
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

	printf("File size %ld, totally add %ld items.\n", file_size, count);
	// KLLSketch *s = kll_sketch_new(k);
	// for (size_t i = 0; i < count; i++) {
	//     kll_sketch_add(s, arr[i]);
	// }

	// double delete_ratio = 0.3;
	// delete_random_elements(s, arr, &count, delete_ratio);

	// kll_sketch_quantiles_test(s, arr, count);

	// kll_sketch_free(s);
	// free(arr);

	// FILE *Efficiency_res;
	// Efficiency_res = fopen(("9.25_Efficiency.txt"), "w");
	FILE *fp_res;
	fp_res = fopen(("9.26_res.txt"), "w");
	// start experiments

	srand(clock() + rand() + time(NULL));
	seed = rand();
	for (int i = 0; i < repeat_parameter; i++)
	{
		begin = clock();
		DDsketch ddsketch1 = DDsketch(0.01, 1024, 1);
		for (int repeat_cnt = 0; repeat_cnt < repeat_times; repeat_cnt++)
		{
			cout << "repeat_cnt: " << repeat_cnt << endl;
			srand(rand());
			seed = rand() + 10;
			ddsketch1.clear_buckets();
			for (unsigned cnt_element = 0; cnt_element < count; cnt_element++)
			{
				ddsketch1.insert(arr[cnt_element]);
			}
			qsort(arr, count, sizeof(double), double_cmp);
			// synthetic dataset
			// for (double q = 0.01; q <= 1; q += 0.01)
			// {
			// 	double est_quantile = ddsketch1.quantile(q);
			// 	double fact_quantile = q * num_elements;
			// 	quantile[unsigned(q * 100 - 1)] = est_quantile;
			// 	errs_quantile[unsigned(q * 100 - 1)] = abs(est_quantile - fact_quantile) / fact_quantile;
			// 	fprintf(fp_res, "%lf,", errs_quantile[unsigned(q * 100 - 1)]);
			// }
			// real dataset
			for (double q = 0.01; q <= 1; q += 0.01)
			{
				double est_quantile = ddsketch1.quantile(q);
				double fact_quantile = arr[unsigned(q * count)];
				quantile[unsigned(q * 100 - 1)] = est_quantile;
				errs_quantile[unsigned(q * 100 - 1)] = abs(est_quantile - fact_quantile) / fact_quantile;
				fprintf(fp_res, "%lf,", errs_quantile[unsigned(q * 100 - 1)]);
			}
			fprintf(fp_res, "\n");
			double max_err = 0;
			double everage_err = 0;
			for (int i = 0; i < 100; i++)
			{
				everage_err += errs_quantile[i];
				if (max_err < errs_quantile[i])
				{
					max_err = errs_quantile[i];
				}
			}
			everage_err = everage_err / 100;
			fprintf(fp_res, "max_err: %lf,", max_err);
			fprintf(fp_res, "everage_err: %lf,", everage_err);
			ddsketch1.print_sketch();
			cout << "everage_err: " << everage_err << endl;
			cout << "max_err: " << max_err << endl;
			fflush(fp_res);
		}
		end = clock();
		cout << "time_total : " << (end - begin) / CLOCKS_PER_SEC << "s" << endl;
		fclose(fp_res);
	}
	free(arr);
	return 0;
}