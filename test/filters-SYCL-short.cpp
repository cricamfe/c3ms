#include "filters-SYCL.hpp"
#include <CL/sycl.hpp>

template <size_t tile_size>
sycl::event pwdist_sycl_tiled() {
    auto t_event = Q.submit([&](sycl::handler &h) {
        h.parallel_for<class Name>(nd_range, [=](sycl::nd_item<2> item) {
            sycl::mad()
            // ...
        });
    });

    return t_event;
}
