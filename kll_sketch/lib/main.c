#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kll_sketch.h"
#include "height.h"

inline static size_t kll_sketch_capacity(KLLSketch * s, size_t h)
{
	double k = s->k;
	double cap = ceil(k * compute_height(s->H - h - 1));
	return (size_t)cap + 1;
}

static void kll_sketch_grow(KLLSketch * sketch)
{
	KLLSketchCompactor * c = (KLLSketchCompactor *)calloc(1, sizeof(KLLSketchCompactor));

	sketch->H += 1;
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

// Add a new value to the sketch
void kll_sketch_update(KLLSketch * sketch, double val)
{
	KLLSketchCompactor * c = sketch->compactors[0];
	kll_compactor_add(c, val);
	sketch->size += 1;
	kll_sketch_compact(sketch);
}

// Create a new sketch
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
