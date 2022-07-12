#include <emmintrin.h>  // __m128i
#include <smmintrin.h>
#include <stdint.h>
#include <xmmintrin.h>  // __m128
#include "kernel_avx2.h"
#include "kernel_sse2.h"
#include "kernel_sse4.h"
#include "ubench.h"

struct Vec2 {
    float x, y;
};

struct Glyph {
    uint16_t x0, y0;
    uint16_t x1, y1;
    int16_t x_offset, y_offset;
    float advance_x;
};

// Vertex layout for textured triangles
struct VertPosUvColor {
    float x;
    float y;
    uint16_t u;
    uint16_t v;
    uint32_t color;
};

namespace ispc {
extern "C" {
extern void write_indexbuffer_ispc_sse2(uint16_t* output, uint16_t start_id, int count);
extern void write_indexbuffer_ispc_sse4(uint16_t* output, uint16_t start_id, int count);
extern void write_indexbuffer_ispc_avx2(uint16_t* output, uint16_t start_id, int count);
extern void write_vertexdata_ispc_sse2(int32_t* output, int32_t* input, struct Vec2& pos, uint64_t count);
extern void write_vertexdata_ispc_sse4(int32_t* output, int32_t* input, struct Vec2& pos, uint64_t count);
extern void write_vertexdata_ispc_avx2(int32_t* output, int32_t* input, struct Vec2& pos, uint64_t count);
}
}  // namespace ispc

const int iteration_count = 100000;
const int vert_iteration_count = 10000;

/*
uint16_t* t = output_data;

for (int i = 0; i < 14; ++i) {
    for (int i = 0; i < 3; ++i) {
        printf("%d ", *t++);
    }
    printf(" - ");

    for (int i = 0; i < 3; ++i) {
        printf("%d ", *t++);
    }

    printf("\n");
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void write_index_buffer_ref(uint16_t* output, uint16_t start_id, int count);
void write_index_buffer_sse2(uint16_t* output, uint16_t start_id, int count);
void write_vertex_data(VertPosUvColor* out, Glyph* g, Vec2 pos, uint64_t count);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TempVertexData {
    Glyph* glyphs;
    VertPosUvColor* output;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TempVertexData create_vert_temp() {
    Glyph* glyphs = new Glyph[vert_iteration_count];

    for (int i = 0; i < vert_iteration_count; ++i) {
        glyphs[i].x0 = i;
        glyphs[i].y0 = i;
        glyphs[i].x1 = i;
        glyphs[i].y1 = i;
        glyphs[i].x_offset = i;
        glyphs[i].y_offset = i;
        glyphs[i].advance_x = 0.01f;
    }

    VertPosUvColor* output = new VertPosUvColor[vert_iteration_count * 4];
    return TempVertexData{glyphs, output};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void destroy_vert_temp(TempVertexData& data) {
    delete[] data.glyphs;
    delete[] data.output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_indexbuffer, c_ref) {
    uint16_t* output_data = new uint16_t[iteration_count * 6];

    UBENCH_DO_BENCHMARK() {
        write_index_buffer_ref(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_indexbuffer, sse2) {
    uint16_t* output_data = new uint16_t[(iteration_count * 6) + 8];

    UBENCH_DO_BENCHMARK() {
        write_index_buffer_sse2(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_indexbuffer, ispc_sse2) {
    int size = (iteration_count * 6) + 128;
    uint16_t* output_data = new uint16_t[size];

    memset(output_data, 0, sizeof(uint16_t) * size);

    UBENCH_DO_BENCHMARK() {
        ispc::write_indexbuffer_ispc_sse2(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_indexbuffer, ispc_sse4) {
    uint16_t* output_data = new uint16_t[(iteration_count * 6) + 128];

    UBENCH_DO_BENCHMARK() {
        ispc::write_indexbuffer_ispc_sse4(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_indexbuffer, ispc_avx2) {
    uint16_t* output_data = new uint16_t[(iteration_count * 6) + 128];
    memset(output_data, 0, 1024);

    UBENCH_DO_BENCHMARK() {
        ispc::write_indexbuffer_ispc_avx2(output_data, 0, iteration_count);
    }

    delete[] output_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertexdata, c_ref) {
    TempVertexData data = create_vert_temp();

    UBENCH_DO_BENCHMARK() {
        write_vertex_data(data.output, data.glyphs, Vec2{0, 0}, vert_iteration_count);
    }

    destroy_vert_temp(data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertexdata, iscp_sse2) {
    TempVertexData data = create_vert_temp();

    UBENCH_DO_BENCHMARK() {
        ispc::Vec2 pos = {0, 0};
        ispc::write_vertexdata_ispc_sse2((int32_t*)data.output, (int32_t*)data.glyphs, pos, vert_iteration_count);
    }

    destroy_vert_temp(data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertexdata, iscp_sse4) {
    TempVertexData data = create_vert_temp();

    UBENCH_DO_BENCHMARK() {
        ispc::Vec2 pos = {0, 0};
        ispc::write_vertexdata_ispc_sse4((int32_t*)data.output, (int32_t*)data.glyphs, pos, vert_iteration_count);
    }

    destroy_vert_temp(data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBENCH_EX(write_vertexdata, iscp_avx2) {
    TempVertexData data = create_vert_temp();

    UBENCH_DO_BENCHMARK() {
        ispc::Vec2 pos = {0, 0};
        ispc::write_vertexdata_ispc_avx2((int32_t*)data.output, (int32_t*)data.glyphs, pos, vert_iteration_count);
    }

    destroy_vert_temp(data);
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

void write_vertex_data(VertPosUvColor* out, Glyph* g, Vec2 pos, uint64_t count) {
    float x_pos = 0.0f;
    const int color = 0x00ff00ff;

    for (uint64_t i = 0; i < count; ++i) {
        const uint16_t x0 = g->x0;
        const uint16_t y0 = g->y0;
        const uint16_t x1 = g->x1;
        const uint16_t y1 = g->y1;
        const int16_t xoff = g->x_offset;
        const int16_t yoff = g->y_offset;

        float rx = xoff + pos.x + x_pos;
        float ry = yoff + pos.y;

        float nx0 = rx;
        float ny0 = ry;
        float nx1 = rx + (x1 - x0);
        float ny1 = ry + (y1 - y0);

        out[0].x = nx0;
        out[0].y = ny0;
        out[0].u = x0;
        out[0].v = y0;
        out[0].color = color;

        out[1].x = nx1;
        out[1].y = ny0;
        out[1].u = x1;
        out[1].v = y0;
        out[1].color = color;

        out[2].x = nx1;
        out[2].y = ny1;
        out[2].u = x1;
        out[2].v = y1;
        out[2].color = color;

        out[3].x = nx0;
        out[3].y = ny1;
        out[3].u = x0;
        out[3].v = y1;
        out[3].color = color;

        x_pos += g->advance_x;

        g++;
        out += 4;
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
