#ifndef KLL_SKETCH_H
#define KLL_SKETCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct
{
	size_t allocated;	// the allocated number of items 
	size_t len;			// the used number of items 
	double *items;
	// bool *flag; 		// sign(element)
} KLLSketchCompactor;

typedef struct
{
	size_t k; 			// k controls the maximum memory used by the stream, which is 3*k + lg(n).
	size_t H; 			// height of sketch (length of compactors), initialized to 1
	size_t max_size;	// the maximun number of items 
	size_t size;		// the used number of items
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
void kll_sketch_update(KLLSketch *sketch, double val, bool flag);
#define kll_sketch_add(sketch, val) kll_sketch_update(sketch, val, true)
#define kll_sketch_delete(sketch, val) kll_sketch_update(sketch, val, false)
void delete_random_elements(KLLSketch * sketch, double *arr, size_t *count, double delete_ratio);

void kll_sketch_free(KLLSketch *sketch);
void kll_sketch_print(KLLSketch *sketch);
void kll_compactor_print(int level, KLLSketchCompactor *c);

KLLQuantiles kll_sketch_get_quantiles(KLLSketch *sketch);
void kll_sketch_quantiles_free(KLLQuantiles q);
double kll_sketch_quantiles_query(KLLQuantiles q, double p);
double kll_sketch_inverse_quantiles_query(KLLQuantiles q, double value);
void kll_sketch_quantiles_test(KLLSketch *sketch, double *arr, size_t arr_len);

#ifdef __cplusplus
}
#endif

#endif //KLL_SKETCH_H
