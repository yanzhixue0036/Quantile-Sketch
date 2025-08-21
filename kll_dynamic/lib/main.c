#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "kll_sketch.h"
// #include "height.h"

static double compute_height(size_t h)
{	
	// c = 2 / 3
	return pow((2.0 / 3.0), (double)h);
}

/*
	Calculate the maximum number of elements, 
	that can be accommodated at the height h.
*/
inline static size_t kll_sketch_capacity(KLLSketch * s, size_t h)
{
	double k = s->k;
	// capacity = kc^{H - h}, where H counts from 1
	double cap = ceil(k * compute_height(s->H - h - 1));
	cap = fmax(cap, 3.0);
	return (size_t)cap;
}

/*
	add a compactor to KLL±.
*/
static void kll_sketch_grow(KLLSketch * sketch)
{
	KLLSketchCompactor * c = (KLLSketchCompactor *)calloc(1, sizeof(KLLSketchCompactor));

	sketch->H += 1;
	// `realloc` extended pointers array
	sketch->compactors = realloc(sketch->compactors, sizeof(KLLSketchCompactor *) * sketch->H);
	sketch->compactors[sketch->H - 1] = c;

	sketch->max_size = 0;
	for (size_t h = 0; h < sketch->H; h++)
	{
		sketch->max_size += kll_sketch_capacity(sketch, h);
	}
}

void kll_compactor_compact(KLLSketchCompactor * c, KLLSketchCompactor * dst);

static void kll_sketch_compact(KLLSketch * sketch)
{
	while (sketch->size >= sketch->max_size)
	{
		for (size_t h = 0; h < sketch->H; h++)
		{
			if (sketch->compactors[h]->len >= kll_sketch_capacity(sketch, h))
			{
				if (h + 1 >= sketch->H)
				{
					// add a new KLLSketchCompactor
					kll_sketch_grow(sketch);
				}

				size_t prev_h = sketch->compactors[h]->len;
				size_t prev_h1 = sketch->compactors[h + 1]->len;

				kll_compactor_compact(sketch->compactors[h], sketch->compactors[h + 1]);

				sketch->size += sketch->compactors[h]->len - prev_h;
				sketch->size += sketch->compactors[h + 1]->len - prev_h1;

				if (sketch->size < sketch->max_size)
					break;
			}
		}
	}
}

void kll_compactor_extend(KLLSketchCompactor * c, size_t extra);
void kll_compactor_add(KLLSketchCompactor * c, double val);

/*
	Add or delete a value to the KLL± sketch.
	param flag: 1 when add else 0
*/
void kll_sketch_update(KLLSketch *sketch, double val, bool flag)
{
	if (!flag) {
        val = -val;
    }
	KLLSketchCompactor * c = sketch->compactors[0];
	kll_compactor_add(c, val);
	sketch->size += 1;
	kll_sketch_compact(sketch);
}

/*  Create a new KLL± sketch.
	param k: 
*/
KLLSketch * kll_sketch_new(int k)
{
	KLLSketch * sketch = (KLLSketch *)calloc(1, sizeof(KLLSketch));
	sketch->k = k;
	kll_sketch_grow(sketch);
	return sketch;
}

void kll_sketch_free(KLLSketch * sketch)
{
	for (size_t i = 0; i < sketch->H; i++)
	{
		free(sketch->compactors[i]);
	}

	free(sketch->compactors);
	free(sketch);
}

void kll_sketch_print(KLLSketch * sketch)
{
	printf("size: %lu\n", sketch->size);
	printf("max size: %lu\n", sketch->max_size);
	printf("compactors:\n");
	for (size_t i = 0; i < sketch->H; i++)
	{
		kll_compactor_print(i, sketch->compactors[i]);
	}
	fflush(stdout);
}

void delete_random_elements(KLLSketch * sketch, double *arr, size_t *count, double delete_ratio) {
    size_t original_count = *count;
    size_t delete_count = original_count * delete_ratio;

    srand(time(NULL));
    char *to_delete = (char *)calloc(original_count, sizeof(char));
    if (to_delete == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < delete_count; i++) {
        size_t index;
        do {
            index = rand() % original_count;
        } while (to_delete[index]);
        to_delete[index] = 1;
    }

    for (size_t i = 0; i < original_count; i++) {
        if (to_delete[i]) {
			kll_sketch_delete(sketch, arr[i]);
            arr[i] = NAN;
        }
    }

    size_t new_count = 0;
    for (size_t i = 0; i < original_count; i++) {
        if (!isnan(arr[i])) {
            arr[new_count++] = arr[i];
        }
    }

    
    free(to_delete);
	*count = new_count;
}