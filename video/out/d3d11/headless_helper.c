#include "headless_helper.h"

bool d3d11_headless_resize(struct ra_ctx* _ra_ctx)
{
    d3d11_headless_priv* priv;
    struct mp_client_api* capi = _ra_ctx->global->client_api;
    struct render_backend* renderer = capi->render_context->renderer;
    priv = renderer->priv;
    struct vo* vo = _ra_ctx->vo;
    if (priv->width != vo->dwidth || priv->height != vo->dheight) {
        vo->dwidth = priv->width;
        vo->dheight = priv->height;
        return true;
    }
    return false;
}

void d3d11_headless_swapchain_out(IDXGISwapChain* _swc, struct ra_ctx* _ra_ctx)
{
    d3d11_headless_priv* priv;
    struct mp_client_api* capi = _ra_ctx->global->client_api;
    struct render_backend* renderer = capi->render_context->renderer;
    priv = renderer->priv;

    if (priv->swc_out != NULL) {
        IDXGISwapChain** pSwapChain = priv->swc_out; // cast void* to specify type
        *pSwapChain = _swc; // could be danger if swc_out is suspension or invalid
    }
}

IDXGISwapChain* mpv_get_swapchain(mpv_handle* ctx)
{
    MPContext* mpctx = ctx->mpctx;
    struct gpu_priv* p = mpctx->video_out->priv;
    struct ra_swapchain* sw = p->ctx->swapchain;
    return sw->priv->swapchain;
}