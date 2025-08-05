#ifdef WM_X11
    #include "wm_x11.c"
#endif

#ifdef WM_WAYLAND
    #error "No wayland support"
#endif

#ifdef WM_WIN32
    #error "No windows support"
#endif

#ifdef WM_APPLE
    #error "No apple support"
#endif
