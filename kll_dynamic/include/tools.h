#ifndef TOOLS
#define TOOLS

#ifdef __cplusplus
extern "C" {
#endif

#define double_EPSILON 1e-6		// precision used for floating point comparisons

int double_cmp(const void * a, const void * b);
int extract_sign_bit_of_double(double value);
#define extract_sign_bit(value) extract_sign_bit_of_double(value)
void write_to_csv(const char *filename, double *row1, double *row2, size_t len);

#ifdef __cplusplus
}
#endif

#endif