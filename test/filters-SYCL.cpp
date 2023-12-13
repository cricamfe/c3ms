#include "filters-SYCL.hpp"

template <size_t tile_size>
sycl::event pwdist_sycl_tiled(float *ptra, float *ptrb, float *out_data, int owidth, int aheight, int awidth, int bheight, int adatawidth, sycl::queue &Q, const std::vector<sycl::event>* depends_on) {
    auto nd_range = sycl::get_nd_range(tile_size, aheight, bheight);
    auto global_range = nd_range.get_group_range();
    auto local_range = nd_range.get_local_range();

    auto M = global_range.get(0);
    auto N = global_range.get(1);

    auto t_event = Q.submit([&](sycl::handler &h) {
        if (depends_on != nullptr && !depends_on->empty()) {
            h.depends_on(*depends_on);
        }
        sycl::local_accessor<float> local_a(tile_size * tile_size, h);
        sycl::local_accessor<float> local_b(tile_size * tile_size, h);

        h.parallel_for(nd_range, [=](sycl::nd_item<2> item) {
            int i = item.get_global_id(0);
            int j = item.get_global_id(1);

            int row = item.get_local_id(0);
            int col = item.get_local_id(1);

            float sum = 0.0;

            for (int kk = 0; kk < adatawidth; kk += tile_size) {
                if (i < aheight && (col + kk) < adatawidth) {
                    local_a[row * tile_size + col] = ptra[i * awidth + (col + kk)];
                }

                if ((row + kk) < bheight && j < owidth) {
                    local_b[row * tile_size + col] = ptrb[(row + kk) * awidth + j];
                }

                item.barrier(sycl::access::fence_space::local_space);
                #pragma unroll
                for (int k = 0; k < tile_size; ++k) {
                    int idx = kk + k;
                    float vec_a, vec_b, diff;
                    if (idx < adatawidth) {
                        vec_a = local_a[row * tile_size + k];
                        vec_b = local_b[col * tile_size + k];
                        diff = vec_a - vec_b;
                        sum = sycl::mad(diff, diff, sum);
                    }
                }
                item.barrier(sycl::access::fence_space::local_space);
            }

            if (i < aheight && j < owidth) {
                out_data[i * owidth + j] = sum;
            }
        });
    });
    if (depends_on == nullptr) {
        t_event.wait();
    }
    
    return t_event;
}
