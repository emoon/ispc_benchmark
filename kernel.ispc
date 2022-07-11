const uniform uint16 lut[8] = {0, 1, 2, 0, 2, 3, 0, 0};

export void write_indexbuffer_ispc(uniform uint16* uniform output, uniform uint16 start_id, uniform int count) {
    uniform uint16 vertex_id = start_id;

    #pragma nounroll
    for (uniform int i = 0; i < count; ++i) {
        
        //assume(((uniform uint16)((void*)output) & (16 * TARGET_WIDTH)-1) != 0);
        //*((uniform uint16<6>* uniform)&output[0]) = vertex_id + lut;

        foreach( j = 0 ... 6)
        {
            output[j] = lut[j] + vertex_id;
        }
        
        vertex_id += 4;
        output += 6;
    }
}