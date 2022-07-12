CXX ?= clang++
ISPC ?= ispc
CXXFLAGS ?= -O3
ISPC_FLAGS ?= -O1 --target=sse2,sse4-i16x8,avx2

ISPC_OBJS = kernel.o kernel_sse2.o kernel_sse4.o kernel_avx2.o 
CPP_OBJS = main.o
OBJS = $(ISPC_OBJS) $(CPP_OBJS) 
ISPC_HEADERS = kernel.h kernel_sse2.h kernel_sse4.h kernel_avx2.h
PROG = ispc_bench

all: $(PROG) 

# Simple "generate .o from .cpp using c++ compiler"
$(CPP_OBJS) : main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate .o and .h +variants using ispc
$(ISPC_OBJS) $(ISPC_HEADERS) : kernel.ispc
	$(ISPC) $(ISPC_FLAGS) -o $@ -h kernel.h $<

clean:
	rm -f $(OBJS) $(ISPC_HEADERS) $(PROG)

$(PROG): $(OBJS)
	$(CXX) -o $@ $^
