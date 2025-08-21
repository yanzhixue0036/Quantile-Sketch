#include "DDsketch.h"
#include <cmath>
#include <iostream>
#include <thread>

DDsketch::DDsketch(double t_alpha, unsigned t_num_buckets, unsigned t_number_of_threads)
{
	alpha = t_alpha;
	gamma = (1 + alpha) / (1 - alpha);
	num_buckets = t_num_buckets;
	buckets = new size_t[num_buckets]{0};
	number_of_threads = t_number_of_threads;
}

int DDsketch::get_bucket_index(double x)
{
	return ceil(log(x) / log(gamma));
}

void DDsketch::insert(double x)
{
	int bucket_index = get_bucket_index(x);
	if(buckets[bucket_index] == 0){
		
	}
	buckets[bucket_index]++;
}

void DDsketch::delete_x(double x)
{
	int bucket_index = get_bucket_index(x);
	buckets[bucket_index]--;
}

double DDsketch::quantile(double q)
{
	size_t count = 0;
	size_t n = 0;

	// 计算所有桶中的总数
	for (size_t i = 0; i < num_buckets; ++i)
	{
		n += buckets[i];
	}

	// 找到对应的quantile
	double judge_count = q * (n - 1);
	for (size_t i = 0; i < num_buckets; ++i)
	{
		count += buckets[i];
		if (count >= judge_count)
		{
			return 2 * pow(gamma, i) / (gamma + 1); // i 是桶的索引
		}
	}

	return -1; // 如果未找到对应的quantile
}

void DDsketch::merge(DDsketch &other)
{
	for (size_t i = 0; i < num_buckets; ++i)
	{
		buckets[i] += other.buckets[i]; // 合并同一索引的桶
	}
}

void DDsketch::print_sketch()
{
	FILE *fp_sk;
	fp_sk = fopen(("9.26_sk.txt"), "w");
	for (size_t i = 0; i < num_buckets; ++i)
	{
		// cout << "Bucket index: " << i << ", Count: " << buckets[i] << endl;
		fprintf(fp_sk, "Bucket index: %zu , Count: %zu\n", i, buckets[i]);
	}
}

void DDsketch::clear_buckets()
{

	for (size_t i = 0; i < num_buckets; ++i)
	{
		buckets[i] = 0; // 合并同一索引的桶
	}
}