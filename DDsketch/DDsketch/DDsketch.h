
#ifndef DDSKETCH_H
#define DDSKETCH_H

// #include "../mingw-std-threads-master/mingw.thread.h"
#include <unordered_map>
#include <vector>
using namespace std;

class DDsketch {
public:
    // unordered_map<int, unsigned> buckets;
    size_t *buckets;      //  the sketch
    double alpha;
    double gamma;
    unsigned num_buckets;
    unsigned number_of_threads;


public:
    DDsketch(double t_alpha, unsigned t_max_buckets, unsigned t_number_of_threads);
    int get_bucket_index(double x);
    void insert(double x);
    void delete_x(double x);
    double quantile(double q);
    void merge(DDsketch &other);
    void clear_buckets();
    void print_sketch();
};

#endif
