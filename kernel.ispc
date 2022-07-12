#if TARGET_WIDTH == 4
    static const varying uint16 LUT0    = {0, 1, 2, 0};
    static const varying uint16 LUT1    = {2, 3, 0, 1};
    static const varying uint16 LUT2    = {2, 0, 2, 3};
    static const varying uint16 VTXID0  = {0, 0, 0, 0};
    static const varying uint16 VTXID1  = {0, 0, 4, 4};
    static const varying uint16 VTXID2  = {4, 4, 4, 4};
#elif TARGET_WIDTH == 8
    static const varying uint16 LUT0    = {0, 1, 2, 0, 2, 3, 0, 1};
    static const varying uint16 LUT1    = {2, 0, 2, 3, 0, 1, 2, 0};
    static const varying uint16 LUT2    = {2, 3, 0, 1, 2, 0, 2, 3};
    static const varying uint16 VTXID0  = {0, 0, 0, 0, 0, 0, 4, 4};
    static const varying uint16 VTXID1  = {4, 4, 4, 4, 8, 8, 8, 8};
    static const varying uint16 VTXID2  = {8, 8, 12, 12, 12, 12, 12, 12};
#elif TARGET_WIDTH == 16
    static const varying uint16 LUT0    = {0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0};
    static const varying uint16 LUT1    = {2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1};
    static const varying uint16 LUT2    = {2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3};
    static const varying uint16 VTXID0  = {0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8};
    static const varying uint16 VTXID1  = {8, 8, 12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 16, 16, 20, 20};
    static const varying uint16 VTXID2  = {20, 20, 20, 20, 24, 24, 24, 24, 24, 24, 28, 28, 28, 28, 28, 28};
#elif TARGET_WIDTH == 32
    static const varying uint16 LUT0    = {0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1};
    static const varying uint16 LUT1    = {2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0};
    static const varying uint16 LUT2    = {2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3};
    static const varying uint16 VTXID0  = {0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 16, 16, 20, 20};
    static const varying uint16 VTXID1  = {20, 20, 20, 20, 24, 24, 24, 24, 24, 24, 28, 28, 28, 28, 28, 28, 32, 32, 32, 32, 32, 32, 36, 36, 36, 36, 36, 36, 40, 40, 40, 40};
    static const varying uint16 VTXID2  = {40, 40, 44, 44, 44, 44, 44, 44, 48, 48, 48, 48, 48, 48, 52, 52, 52, 52, 52, 52, 56, 56, 56, 56, 56, 56, 60, 60, 60, 60, 60, 60};
#else
    #error Unsupported TARGET_WIDTH
#endif

// count must be >= programCount
// use unmasked to simplify the code generation, and remove export versions and masked versions
export void write_indexbuffer_ispc(uniform uint16* uniform output, uniform uint16 start_id, uniform int32 count) {
    uniform uint16 vertex_id = start_id;

    uniform int32 step = programCount * 3 / 6;
    
    #pragma nounroll
    for (uniform int64 i = 0; i < count; i += step)
    {   
        output[programIndex] = LUT0 + VTXID0 + vertex_id;
        output += programCount;
        output[programIndex] = LUT1 + VTXID1 + vertex_id;
        output += programCount;
        output[programIndex] = LUT2 + VTXID2 + vertex_id;
        output += programCount;

        vertex_id += programCount*2;
    }
}

struct Vec2 {
    float x, y;
};

export void write_vertexdata_ispc(uniform int32 output[], uniform int32 input[], uniform Vec2& pos, uniform uint64 count) {
    varying float x_pos = 0.0f;
    uniform int pos_index = 0;
    uniform int32 color = 0x00ff00ff;

    for (uniform uint64 i = 0; i < count; i += programCount) {
        int32 x0y0;
        int32 x1y1;
        int32 xy_offset;
        int32 int_x_adv; // really a float, but loaded as int and bit-casted to float

        aos_to_soa4(&input[i * programCount], &x0y0, &x1y1, &xy_offset, &int_x_adv);

        // Extract data 
        float x0 = (float)(x0y0 >> 16);
        float y0 = (float)(x0y0 & 0xffff); 
        float x1 = (float)(x1y1 >> 16);
        float y1 = (float)(x1y1 & 0xffff); 

        int16 xoff = xy_offset >> 16;
        int16 yoff = xy_offset & 0xffff;
        float x_adv = floatbits(int_x_adv);

        float rx = xoff + pos.x + x_pos;
        float ry = yoff + pos.y;

        float nx0 = rx;
        float ny0 = ry;
        float nx1 = rx + (x1 - x0);
        float ny1 = ry + (y1 - y0);

        int32 x1y0 = ((x1y1 & 0xffff0000) | (x0y0 & 0x00000ffff));
        int32 x0y1 = ((x0y0 & 0xffff0000) | (x1y1 & 0x00000ffff));

        soa_to_aos4(nx0, ny0, x0y0, color, &output[(i * programCount) + 0]);
        soa_to_aos4(nx1, ny0, x1y0, color, &output[(i * programCount) + 4]);
        soa_to_aos4(nx1, ny1, x1y1, color, &output[(i * programCount) + 8]);
        soa_to_aos4(nx0, ny1, x0y1, color, &output[(i * programCount) + 12]);

        x_pos += x_adv; 
    }
}



