#include "custom_helper.h"

bool is_custom_device(struct mpv_global* _global)
{
	MPContext* mpctx = _global->client_api->mpctx;
	if (mpctx->custom_d3d11device) {
		return true;
	}
	return false;
}

bool bind_device(struct mpv_global* _global, ID3D11Device** _dev)
{
	MPContext* mpctx = _global->client_api->mpctx;
	ID3D11Device* dev = (ID3D11Device*)mpctx->d3d11_device;
	*_dev = dev;
	return true;
}

void* get_device(struct mpv_global* _global)
{
	MPContext* mpctx = _global->client_api->mpctx;
	if (mpctx->custom_d3d11device) {
		ID3D11Device* dev = (ID3D11Device*)mpctx->custom_d3d11device;
		return (void*)dev;
	}
	return NULL;
}

void mpv_set_custom_d3d11device(mpv_handle* ctx, ID3D11Device* d3d11device)
{
	MPContext* mpctx = ctx->mpctx;
	mpctx->custom_d3d11device = true;
	mpctx->d3d11_device = d3d11device;
}

IDXGISwapChain* mpv_get_swapchain(mpv_handle* ctx)
{
	MPContext* mpctx = ctx->mpctx;
	struct gpu_priv* p = mpctx->video_out->priv;
	struct ra_swapchain* sw = p->ctx->swapchain;
	return sw->priv->swapchain;
}