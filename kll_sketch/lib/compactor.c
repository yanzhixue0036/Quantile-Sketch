#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "kll_sketch.h"

// coin is a simple struct to let us get random bools and make minimum calls
// to the random number generator.
typedef struct
{
	uint64_t st;
	uint64_t mask;
} coin;


// 64-bit xorshift multiply rng from http://vigna.di.unimi.it/ftp/papers/xorshift.pdf
static uint64_t xor_shift_mult64(uint64_t x)
{
	x ^= x >> 12; // a
	x ^= x << 25; // b
	x ^= x >> 27; // c
	return x * 2685821657736338717;
}

// returns either 0 or 1
static int toss()
{
	int res = 0;
	static coin c = {0, 0};

	if (c.mask == 0)
	{
		if (c.st == 0)
			c.st = rand();

		c.st = xor_shift_mult64(c.st);
		c.mask = 1;
	}

	if ((c.st & c.mask) > 0)
		res = 1;

	c.mask <<= 1;
	return res;
}

void kll_compactor_print(int level, KLLSketchCompactor *c)
{
    printf("compactor %d: ", level);
    for (size_t i = 0; i < c->len; i++)
    {
        printf("%f ", c->items[i]);
    }
    printf("\n");
}

static int cmp(const void * a, const void * b)
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

static void quicksort(KLLSketchCompactor * c)
{
	qsort(c->items, c->len, sizeof(double), cmp);
}

static void simple_sort(KLLSketchCompactor * c)
{
	for (size_t i = 0; i < c->len - 1; i++)
	{
		for (size_t j = i + 1; j < c->len; j++)
			if (c->items[i] > c->items[j])
			{
				double tmp = c->items[i];
				c->items[i] = c->items[j];
				c->items[j] = tmp;
			}
	}
}

void kll_compactor_extend(KLLSketchCompactor * c, size_t extra)
{
	c->allocated += extra;
	c->items = (double *)realloc(c->items, sizeof(double) * c->allocated);
}

void kll_compactor_add(KLLSketchCompactor * c, double val)
{
	if (c->allocated <= c->len)
		kll_compactor_extend(c, 10);

	c->items[c->len] = val;
	c->len += 1;
}

void kll_compactor_compact(KLLSketchCompactor * c, KLLSketchCompactor * dst)
{
	if (c->len > 1)
	{
		// sort
		if (c->len == 2)
		{
			if (c->items[0] > c->items[1])
			{
				double tmp = c->items[0];
				c->items[0] = c->items[1];
				c->items[1] = tmp;
			}
		}
		else if (c->len > 100)
			quicksort(c);
		else
			simple_sort(c);
	}

	size_t free = dst->allocated - dst->len;
	if (free < c->len / 2)
		kll_compactor_extend(dst, c->len / 2 - free);

	for (size_t i = toss(); i < c->len; i += 2)
		kll_compactor_add(dst, c->items[i]);

	c->len = 0;
}
