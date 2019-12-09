#include "headless_helper.h"

IDXGISwapChain* mpv_get_swapchain(mpv_handle* ctx)
{
    MPContext* mpctx = ctx->mpctx;
    struct gpu_priv* p = mpctx->video_out->priv;
    struct ra_swapchain* sw = p->ctx->swapchain;
    return sw->priv->swapchain;
}