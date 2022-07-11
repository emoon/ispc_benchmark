CXX ?= clang++
ISPC ?= ispc
CXXFLAGS ?= -O3
# ISPC_FLAGS ?= --target=sse2,sse4,avx2

ISPC_OBJS = kernel_sse2.o kernel_sse4.o kernel_avx2.o 
CPP_OBJS = main.o
OBJS = $(ISPC_OBJS) $(CPP_OBJS) 
ISPC_HEADERS = kernel_sse2.h kernel_sse4.h kernel_avx2.h
PROG = ispc_bench

all: $(PROG) 

# Simple "generate .o from .cpp using c++ compiler"
$(CPP_OBJS) : main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate .o and .h for sse2
kernel_sse2.o kernel_sse2.h : kernel.ispc
	$(ISPC) --target=sse2 -o $@ -h kernel_sse2.h $<

# Generate .o and .h for sse4 
kernel_sse4.o kernel_sse4.h : kernel.ispc
	$(ISPC) --target=sse4 -o $@ -h kernel_sse4.h $<

# Generate .o and .h for sse2
kernel_avx2.o kernel_avx2.h : kernel.ispc
	$(ISPC) --target=avx2 -o $@ -h kernel_avx2.h $<

clean:
	rm -f $(OBJS) $(ISPC_HEADERS) $(PROG)

$(PROG): $(OBJS)
	$(CXX) -o $@ $^