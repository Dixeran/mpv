#include "config.h"
#include "video/out/gpu/hwdec.h"
#include "libmpv/render_gl.h"
#include "video/out/gpu/video.h"
#include "video/out/libmpv.h"

// this should be exposed to d3d11 context_headless, 
// and should be retrived when "control" method called.
typedef struct d3d11_headless_priv {
    int width;
    int height;
    // pointer to user's swapchain pointer
    // IDXGISwapChain**
    void* swc_out;
} d3d11_headless_priv;

static int init(struct render_backend* ctx, mpv_render_param* params)
{
    char* api = get_mpv_render_param(params, MPV_RENDER_PARAM_API_TYPE, NULL);
    if (!api) {
        return MPV_ERROR_INVALID_PARAMETER;
    }

    if (strcmp(api, MPV_RENDER_API_TYPE_D3D11_HEADLESS) != 0) {
        return MPV_ERROR_NOT_IMPLEMENTED;
    }

    ctx->priv = talloc_zero(NULL, d3d11_headless_priv);
    d3d11_headless_priv* m_priv = ctx->priv;
    m_priv->width = 320;
    m_priv->height = 240;
    
    void* swc_out = get_mpv_render_param(params, MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, NULL);
    if (swc_out) {
        m_priv->swc_out = swc_out;
    }
    return 0;
}

static bool check_format(struct render_backend* ctx, int imgfmt)
{
    return true;
}

typedef struct render_size {
    int width;
    int height;
} render_size;

static int set_parameter(struct render_backend* ctx, mpv_render_param param) {
    // use this method to update render size things.
    render_size* new_size = param.data;
    d3d11_headless_priv* priv = ctx->priv;
    priv->width = new_size->width;
    priv->height = new_size->height;
    return 0;
}

static void destroy(struct render_backend* ctx)
{
    // TODO: destroy me
}

const struct render_backend_fns render_backend_d3d11_headless = {
    .init = init,
    .check_format = check_format,
    .set_parameter = set_parameter,
    .reconfig = NULL,
    .reset = NULL,
    .update_external = NULL,
    .resize = NULL,
    .get_target_size = NULL,
    .render = NULL,
    .get_image = NULL,
    .screenshot = NULL,
    .perfdata = NULL,
    .destroy = destroy,
};