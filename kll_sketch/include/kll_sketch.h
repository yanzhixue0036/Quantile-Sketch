#ifndef KLL_SKETCH_H
#define KLL_SKETCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct
{
	size_t allocated;
	size_t len;
	double *items;
} KLLSketchCompactor;

typedef struct
{
	size_t k; // k controls the maximum memory used by the stream, which is 3*k + lg(n).
	size_t max_size;
	size_t H; // height of sketch (length of compactors)
	size_t size;
	KLLSketchCompactor ** compactors;
} KLLSketch;

typedef struct
{
    double w;
    double v;
} KLLQuantile;

typedef struct {
    size_t len;
    KLLQuantile *quantiles;
} KLLQuantiles;

KLLSketch *kll_sketch_new(int k);
void kll_sketch_update(KLLSketch *sketch, double val);
void kll_sketch_free(KLLSketch *sketch);

void kll_sketch_print(KLLSketch *sketch);
void kll_compactor_print(int level, KLLSketchCompactor *c);

KLLQuantiles kll_sketch_get_quantiles(KLLSketch *sketch);
void kll_sketch_quantiles_free(KLLQuantiles q);
double kll_sketch_quantiles_query(KLLQuantiles q, double p);

#ifdef __cplusplus
}
#endif

#endif //KLL_SKETCH_H
