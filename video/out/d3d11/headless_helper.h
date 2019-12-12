#ifndef MP_HEADLESS_HELPER_H
#define MP_HEADLESS_HELPER_H

#include <d3d11.h>
#include <pthread.h>

#include "player/core.h"
#include "player/client.h"
#include "common/global.h"
#include "osdep/atomic.h"
#include "video/out/vo.h"
#include "video/out/gpu/context.h"
#include "video/out/libmpv.h"

struct mp_client_api {
    struct MPContext* mpctx;

    pthread_mutex_t lock;

    atomic_bool uses_vo_libmpv;

    // -- protected by lock

    struct mpv_handle** clients;
    int num_clients;
    bool shutting_down; // do not allow new clients
    bool have_terminator; // a client took over the role of destroying the core
    bool terminate_core_thread; // make libmpv core thread exit

    struct mp_custom_protocol* custom_protocols;
    int num_custom_protocols;

    struct mpv_render_context* render_context;
    struct mpv_opengl_cb_context* gl_cb_ctx;
};

struct mpv_handle {
    // -- immmutable
    char name[MAX_CLIENT_NAME];
    struct mp_log* log;
    struct MPContext* mpctx;
    struct mp_client_api* clients;

    // -- not thread-safe
    struct mpv_event* cur_event;
    struct mpv_event_property cur_property_event;

    pthread_mutex_t lock;

    pthread_mutex_t wakeup_lock;
    pthread_cond_t wakeup;

    // -- protected by wakeup_lock
    bool need_wakeup;
    void (*wakeup_cb)(void* d);
    void* wakeup_cb_ctx;
    int wakeup_pipe[2];

    // -- protected by lock

    uint64_t event_mask;
    bool queued_wakeup;
    int suspend_count;

    mpv_event* events;      // ringbuffer of max_events entries
    int max_events;         // allocated number of entries in events
    int first_event;        // events[first_event] is the first readable event
    int num_events;         // number of readable events
    int reserved_events;    // number of entries reserved for replies
    size_t async_counter;   // pending other async events
    bool choked;            // recovering from queue overflow

    struct observe_property** properties;
    int num_properties;
    int lowest_changed;     // attempt at making change processing incremental
    uint64_t property_event_masks; // or-ed together event masks of all properties

    bool fuzzy_initialized; // see scripting.c wait_loaded()
    bool is_weak;           // can not keep core alive on its own
    struct mp_log_buffer* messages;
};

struct gpu_priv {
    struct mp_log* log;
    struct ra_ctx* ctx;

    char* context_name;
    char* context_type;
    struct ra_ctx_opts opts;
    struct gl_video* renderer;

    int events;
};

struct priv {
    struct d3d11_opts* opts;

    struct ra_tex* backbuffer;
    ID3D11Device* device;
    IDXGISwapChain* swapchain;
    struct mp_colorspace swapchain_csp;

    int64_t perf_freq;
    unsigned last_sync_refresh_count;
    int64_t last_sync_qpc_time;
    int64_t vsync_duration_qpc;
    int64_t last_submit_qpc;
};

struct mpv_render_context {
    struct mp_log* log;
    struct mpv_global* global;
    struct mp_client_api* client_api;

    atomic_bool in_use;

    // --- Immutable after init
    struct mp_dispatch_queue* dispatch;
    bool advanced_control;
    struct dr_helper* dr;           // NULL if advanced_control disabled

    pthread_mutex_t control_lock;
    // --- Protected by control_lock
    mp_render_cb_control_fn control_cb;
    void* control_cb_ctx;

    pthread_mutex_t update_lock;
    pthread_cond_t update_cond;     // paired with update_lock

    // --- Protected by update_lock
    mpv_render_update_fn update_cb;
    void* update_cb_ctx;

    pthread_mutex_t lock;
    pthread_cond_t video_wait;      // paired with lock

    // --- Protected by lock
    struct vo_frame* next_frame;    // next frame to draw
    int64_t present_count;          // incremented when next frame can be shown
    int64_t expected_flip_count;    // next vsync event for next_frame
    bool redrawing;                 // next_frame was a redraw request
    int64_t flip_count;
    struct vo_frame* cur_frame;
    struct mp_image_params img_params;
    int vp_w, vp_h;
    bool flip;
    bool imgfmt_supported[IMGFMT_END - IMGFMT_START];
    bool need_reconfig;
    bool need_resize;
    bool need_reset;
    bool need_update_external;
    struct vo* vo;

    // --- Mostly immutable after init.
    struct mp_hwdec_devices* hwdec_devs;

    // --- All of these can only be accessed from mpv_render_*() API, for
    //     which the user makes sure they're called synchronized.
    struct render_backend* renderer;
    struct m_config_cache* vo_opts_cache;
    struct mp_vo_opts* vo_opts;
};

// same as priv in libmpv_d3d11_headless.c
typedef struct d3d11_headless_priv {
    int width;
    int height;
    // pointer to user's swapchain pointer
    // IDXGISwapChain**
    void* swc_out;
} d3d11_headless_priv;

bool d3d11_headless_resize(struct ra_ctx* _ra_ctx);

void d3d11_headless_swapchain_out(IDXGISwapChain* _swc, struct ra_ctx* _ra_ctx);

IDXGISwapChain* mpv_get_swapchain(mpv_handle* ctx);

#endif // !MP_HEADLESS_HELPER_H
