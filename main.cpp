#include <emmintrin.h>  // __m128i
#include <smmintrin.h>
#include <stdint.h>
#include <xmmintrin.h>  // __m128
#include "kernel_avx2.h"
#include "kernel_sse2.h"
#include "kernel_sse4.h"
#include "ubench.h"

namespace ispc {
extern "C" {
extern void write_indexbuffer_ispc_sse2(uint16_t* output, uint16_t start_id, int count);
extern void write_indexbuffer_ispc_sse4(uint16_t* output, uint16_t start_id, int count);
extern void write_indexbuffer_ispc_avx2(uint16_t* output, uint16_t start_id, int count);
}
}  // namespace ispc

const int iteration_count = 10000;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void write_index_buffer_ref(uint16_t* output, uint16_t start_id, int count);
void write_index_buffer_sse2(uint16_t* output, uint16_t start_id, int count);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertices, c_ref) {
    uint16_t* output_data = new uint16_t[iteration_count * 6];

    UBENCH_DO_BENCHMARK() {
        write_index_buffer_ref(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertices, manual_sse2) {
    uint16_t* output_data = new uint16_t[(iteration_count * 6) + 8];

    UBENCH_DO_BENCHMARK() {
        write_index_buffer_sse2(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertices, ispc_sse2) {
    uint16_t* output_data = new uint16_t[(iteration_count * 6) + 128];

    UBENCH_DO_BENCHMARK() {
        ispc::write_indexbuffer_ispc_sse2(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertices, ispc_sse4) {
    uint16_t* output_data = new uint16_t[(iteration_count * 6) + 128];

    UBENCH_DO_BENCHMARK() {
        ispc::write_indexbuffer_ispc_sse4(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertices, ispc_avx2) {
    uint16_t* output_data = new uint16_t[(iteration_count * 6) + 128];

    UBENCH_DO_BENCHMARK() {
        ispc::write_indexbuffer_ispc_avx2(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

UBENCH_MAIN();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// c-ref implementation

void write_index_buffer_ref(uint16_t* output, uint16_t start_id, int count) {
    uint16_t vertex_id = start_id;

    for (int i = 0; i < count; ++i) {
        output[0] = vertex_id + 0;
        output[1] = vertex_id + 1;
        output[2] = vertex_id + 2;

        output[3] = vertex_id + 0;
        output[4] = vertex_id + 2;
        output[5] = vertex_id + 3;

        vertex_id += 4;
        output += 6;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// manual sse2 implementation

void write_index_buffer_sse2(uint16_t* output, uint16_t start_id, int count) {
    __m128i vertex_id = _mm_set1_epi16(start_id);
    __m128i index_add = _mm_set1_epi16(4);

    // this is really 0,1,2 0,2,3 but defined in reverse order
    __m128i index_temp_add = _mm_set_epi16(0, 0, 3, 2, 0, 2, 1, 0);

    for (int i = 0; i < count; ++i) {
        __m128i t = _mm_add_epi16(vertex_id, index_temp_add);
        _mm_storeu_si128((__m128i*)output, t);
        vertex_id = _mm_add_epi16(vertex_id, index_add);
        output += 6;
    }
}
