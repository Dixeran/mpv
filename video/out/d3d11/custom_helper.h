#pragma once

#include <d3d11.h>
#include <pthread.h>

#include "../../../player/core.h"
#include "../../../player/client.h"
#include "../../../common/global.h"
#include "../../../osdep/atomic.h"

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

/* Used by internal to determin if custom d3d11 device is injected. */
bool is_custom_device(struct mpv_global* _global);

/* Used by internal to set device from MPContext. */
bool bind_device(struct mpv_global* _global, ID3D11Device** _dev);

/* Used by internal to get custom device's pointer if injected. */
void* get_device(struct mpv_global* _global);


//bool custom_d3d11_create_swapchain();


/* Following functions should be export. */

/* Used by user to inject custom device */
void mpv_set_custom_d3d11device(mpv_handle* ctx, ID3D11Device* d3d11device);