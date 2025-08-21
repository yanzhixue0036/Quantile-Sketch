#include "assert.h"
#include "stdlib.h"
#include "math.h"
#include "kll_sketch.h"

static int cmp(const void * a, const void * b)
{
	double first = ((KLLQuantile *)a)->v;
	double second = ((KLLQuantile *)b)->v;

	if (fabs(first - second) < 0.0001)
		return 0;
	else if (first < second)
		return -1;
	else
		return 1;
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
			q.quantiles[idx].w = weight;
			q.quantiles[idx].v = c->items[j];
			idx += 1;
		}
		total_weight += weight * c->len;
	}

	qsort(q.quantiles, q.len, sizeof(KLLQuantile), cmp);

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
