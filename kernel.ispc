/*
const uniform uint16 lut[8] = {0, 1, 2, 0, 2, 3, 0, 0};

export void write_indexbuffer_ispc(uniform uint16* uniform output, uniform uint16 start_id, uniform int count) {
    uniform uint16 vertex_id = start_id;

    #pragma nounroll
    for (uniform int i = 0; i < count; ++i) {
        
        //assume(((uniform uint16)((void*)output) & (16 * TARGET_WIDTH)-1) != 0);
        //((uniform uint16<6>* uniform)&output[0]) = vertex_id + lut;

        foreach( j = 0 ... 6)
        {
            output[j] = lut[j] + vertex_id;
        }
        
        vertex_id += 4;
        output += 6;
    }
}
*/

#if TARGET_WIDTH == 4
static const varying uint16 LUT0 = {0, 1, 2, 0};
static const varying uint16 LUT1 = {2, 3, 0, 1};
static const varying uint16 LUT2 = {2, 0, 2, 3};
static const varying uint16 VTXID0 = {0, 0, 0, 0};
static const varying uint16 VTXID1 = {0, 0, 4, 4};
static const varying uint16 VTXID2 = {4, 4, 4, 4};
#elif TARGET_WIDTH == 8
static const varying uint16 LUT0 = {0, 1, 2, 0, 2, 3, 0, 0};
static const varying uint16 LUT1 = {2, 3, 0, 1, 0, 0, 2, 3};
static const varying uint16 LUT2 = {2, 0, 2, 3, 2, 0, 2, 3};
static const varying uint16 VTXID0 = {0, 0, 0, 0, 0, 0, 0, 0};
static const varying uint16 VTXID1 = {0, 0, 1, 1, 0, 0, 1, 1};
static const varying uint16 VTXID2 = {1, 1, 1, 1, 1, 1, 1, 1};
#elif TARGET_WIDTH == 16
#else
#error Unsupported TARGET_WIDTH
#endif


export void write_indexbuffer_ispc(uniform uint16* uniform output, uniform uint16 start_id, uniform int32 count) {
    uniform uint16 vertex_id = start_id;

#if TARGET_WIDTH == 8
	count = count >> 1;
#endif
    
    for (uniform int32 i = 0; i < count; i+=2)
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

