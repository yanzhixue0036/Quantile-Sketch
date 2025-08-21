#include "assert.h"
#include "stdlib.h"
#include <stdio.h>
#include "math.h"
#include "tools.h"
#include "kll_sketch.h"

static int KLLQuantile_cmp(const void * a, const void * b)
{
	double first = ((KLLQuantile *)a)->v;
	double second = ((KLLQuantile *)b)->v;

	if (fabs(first - second) < double_EPSILON)
		return 0;
	else if (first < second)
		return -1;
	else
		return 1;
}

void simplify_KLLQuantiles(KLLQuantiles *q) {
    if (q->len <= 1) return;

    size_t new_len = 0;
    KLLQuantile *simplified_quantiles = malloc(sizeof(KLLQuantile) * q->len);
    if (simplified_quantiles == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < q->len; ++i) {
        if (i > 0 && q->quantiles[i].v == q->quantiles[i - 1].v) {
            simplified_quantiles[new_len - 1].w += q->quantiles[i].w;
        } else {
            // simplified_quantiles[new_len].v = q->quantiles[i].v;
			// simplified_quantiles[new_len].w = q->quantiles[i].w;
			simplified_quantiles[new_len] = q->quantiles[i];
            ++new_len;
        }
    }

    free(q->quantiles);
    q->quantiles = simplified_quantiles;
    q->len = new_len;
}

KLLQuantiles kll_sketch_get_quantiles(KLLSketch * sketch)
{
	KLLQuantiles q = {sketch->size, NULL};
	q.quantiles = (KLLQuantile *)malloc(sizeof(KLLQuantile) * sketch->size);

	size_t idx = 0;
	double total_weight = 0;

	for (size_t i = 0; i < sketch->H; i++)
	{
		double weight = 1 << i;
		KLLSketchCompactor * c = sketch->compactors[i];
		for (size_t j = 0; j < c->len; j++)
		{
			assert(idx < sketch->size);
			if (extract_sign_bit_of_double(c->items[j]) == 0)
			{
				total_weight += weight;
				q.quantiles[idx].w = weight;
				q.quantiles[idx].v = c->items[j];
			}
			else
			{
				total_weight -= weight;
				q.quantiles[idx].w = -weight;
				q.quantiles[idx].v = fabs(c->items[j]);
			}
			idx += 1;
			
		}
	}

	qsort(q.quantiles, q.len, sizeof(KLLQuantile), KLLQuantile_cmp);
	simplify_KLLQuantiles(&q);

	double cur_weight = 0;
	for (size_t i = 0; i < q.len; i++)
	{
		cur_weight += q.quantiles[i].w;
		q.quantiles[i].w = cur_weight / total_weight;
	}

	return q;
}

void kll_sketch_quantiles_free(KLLQuantiles q)
{
	free(q.quantiles);
}

// get a quantile value
double kll_sketch_quantiles_query(KLLQuantiles q, double p)
{
	if (q.len == 0)
		return 0;

	for (size_t j = 0; j < q.len; j++)
	{
		if (q.quantiles[j].w >= p)
			return q.quantiles[j].v;
	}
	return q.quantiles[q.len - 1].v;
}

// get a quantile
double kll_sketch_inverse_quantiles_query(KLLQuantiles q, double value)
{
	if (q.len == 0)
		return 1;
	
	for (size_t j = 0; j < q.len; j++)
	{
		if (q.quantiles[j].v >= value)
			return q.quantiles[j].w;
	}
	return q.quantiles[q.len - 1].w;
}

double calculate_inverse_quantile(double* arr, int n, double val) {
    double rank = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] <= val) {
            rank++;
        }else{
			break;
		}
    }
    return rank / n;
}

// Quantiles Query Relative Error
static double quantiles_query_relative_error(double actual, double estimate){
	double req = (actual != 0.0)?
		fabs(actual - estimate) / fmax(actual, estimate) : 0.0;
	return req;
}

// Rank Error
static double quantiles_query_rank_error(double *arr, int len, double actual, double estimate){
	// double actual_quantile = kll_sketch_inverse_quantiles_query(q, actual);
	// double estimate_quantile = kll_sketch_inverse_quantiles_query(q, estimate);
	double actual_quantile = calculate_inverse_quantile(arr, len, actual);
	double estimate_quantile = calculate_inverse_quantile(arr, len, estimate);
	// printf("%f - %f, diff=%f\n", actual_quantile, estimate_quantile, actual_quantile - estimate_quantile);
	return fabs(actual_quantile - estimate_quantile);
}

// Quantiles Query Ratio Error
static double quantiles_query_ratio_error(double actual, double estimate){
	double req = (fmin(actual, estimate) != 0.0)?
		fmax(actual, estimate) / fmin(actual, estimate) : 0.0;
	return req;
}

void kll_sketch_quantiles_test(KLLSketch *sketch, double *arr, size_t arr_len)
{
    //kll_sketch_print(sketch);
    qsort(arr, arr_len, sizeof(double), double_cmp);
    KLLQuantiles q = kll_sketch_get_quantiles(sketch);

	size_t res_len = 102;
	double quantiles[res_len], errors[res_len];
    double total_error = 0.0;
	size_t count = 0;
	errors[res_len - 1] = 0;

	// quantiles[0] = 0.01;
	// errors[0] = 0;
    for(double i = 0.01; i < 1; i += 0.01){
        size_t rank = i * arr_len;
        double actual = arr[rank];
        double estimate = kll_sketch_quantiles_query(q, i);

		// Relative Error
		// double error = quantiles_query_relative_error(actual, estimate);
		// Rank Error
		double error = quantiles_query_rank_error(arr, arr_len, actual, estimate);
		// Ratio Error
		// double error = quantiles_query_ratio_error(actual, estimate);

        printf("%f - %f, diff=%f\n", actual, estimate, actual - estimate);
        printf("Quantile %f error = %f\n", i, error);
        //ASSERT_TRUE(error < 0.01);

		quantiles[count] = i;
        errors[count] = error;
		if (error > errors[res_len - 1]) {
			quantiles[res_len-1] = i;
            errors[res_len - 1] = error;
        }
		total_error += error;
        count++;
    }
	errors[res_len - 2] = total_error / (double)count;
	errors[res_len - 3] = 0.0, quantiles[res_len - 3] = 0.0;
	printf("Average quantile error = %f\n", errors[res_len - 2]);
	printf("Max quantile error = %f\n", errors[res_len - 1]);

	char filename[30];
	int log_k = (int)log2(sketch->k);
	int log_size = (int)log2(sketch->size);
	snprintf(filename, sizeof(filename), "k=2^%d,size=2^%d.csv", log_k, log_size);
	write_to_csv(filename, quantiles, errors, res_len);
}