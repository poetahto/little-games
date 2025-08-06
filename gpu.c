#ifdef GPU_OPENGL
    #include "gpu_opengl.c"
#endif

#ifdef GPU_VULKAN
    #error "Vulkan not supported"
#endif

#ifdef GPU_D3D11
    #error "D3D11 not supported"
#endif

#ifdef GPU_METAL
    #error "Metal not supported"
#endif
