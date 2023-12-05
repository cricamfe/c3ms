#include "filters-AVX.hpp"

void cosine_filter_AVX(const int filter_h, const int filter_w) {
	//do convolution
	const int apron_y = filter_h / 2;
	const int apron_x = filter_w / 2;
	int* pixel_offsets=(int*) malloc(sizeof(int)*2000);
	float* fr_ptr;
	int width = 2400;
	for (int j=apron_x; j<(width - apron_x); j++ ) {
		__m256 image_cache0 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[0]]);
		__m256 image_cache1 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[1]]);
		__m256 image_cache2 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[2]]);
		__m256 image_cache3 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[3]]);
		__m256 image_cache4 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[4]]);
		__m256 image_cache5 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[5]]);
		__m256 image_cache6 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[6]]);
		__m256 image_cache7 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[7]]);
		__m256 image_cache8 = _mm256_broadcast_ss(&fr_ptr[pixel_offsets[8]]);
	}
	__m256 temp_sum2 = _mm256_insertf128_ps(temp_sum,
		_mm256_extractf128_ps(temp_sum, 0), 1);
}