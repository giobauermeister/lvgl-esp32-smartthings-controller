/**
 * @file lottie_player.c
 *
 * Hosts an lv_lottie, which LVGL Pro XML has no tag for yet. LVGL (and the
 * Editor's preview runtime) must be built with LV_USE_LOTTIE and ThorVG.
 *
 * The lv_lottie child is deleted and created again on every restart. LVGL frees
 * the lv_anim_t of a finished non-looping animation, but lv_lottie keeps the
 * pointer, so reusing the child after that would write to freed memory. The
 * original UI recreated its Lottie objects for the same reason.
 */

/*********************
 *      INCLUDES
 *********************/

#include "lottie_player_private_gen.h"
#include "../../smartthings_ui.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void update(lottie_player_t * widget, bool restart);
static void anim_completed_cb(lv_anim_t * a);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lottie_player_constructor_hook(lv_obj_t * obj)
{
    lottie_player_t * widget = (lottie_player_t *)obj;

    /* Everything else starts zeroed: no source, no buffer, no child */
    widget->loop = true;
}

void lottie_player_destructor_hook(lv_obj_t * obj)
{
    lottie_player_t * widget = (lottie_player_t *)obj;

    /* The lv_lottie child is already deleted with the other children */
    lv_free(widget->buf);
    widget->buf = NULL;
}

void lottie_player_event_hook(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_SIZE_CHANGED) {
        update((lottie_player_t *)lv_event_get_current_target(e), false);
    }
}

void lottie_player_set_src(lv_obj_t * lottie_player, const char * src)
{
    lottie_player_t * widget = (lottie_player_t *)lottie_player;

    widget->asset = lottie_assets_find(src);
    if(widget->asset == NULL && src && src[0]) LV_LOG_WARN("no Lottie animation named \"%s\"", src);

    /* src may be a temporary string (the XML parser's), so keep the asset's own name */
    widget->src = widget->asset ? widget->asset->name : NULL;
    update(widget, true);
}

void lottie_player_set_loop(lv_obj_t * lottie_player, bool loop)
{
    lottie_player_t * widget = (lottie_player_t *)lottie_player;

    widget->loop = loop;
    update(widget, true);
}

void lottie_player_play(lv_obj_t * lottie_player)
{
    update((lottie_player_t *)lottie_player, true);
}

void lottie_player_set_ready_cb(lv_obj_t * lottie_player, lottie_player_ready_cb_t ready_cb, void * user_data)
{
    lottie_player_t * widget = (lottie_player_t *)lottie_player;

    widget->ready_cb = ready_cb;
    widget->ready_user_data = user_data;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Make the child match the source and the content size. A new size needs a new
 * buffer; restart (a new source or loop mode) rebuilds the child even when the
 * size is unchanged. */
static void update(lottie_player_t * widget, bool restart)
{
    lv_obj_t * obj = (lv_obj_t *)widget;
    int32_t w = lv_obj_get_content_width(obj);
    int32_t h = lv_obj_get_content_height(obj);
    bool resized = widget->buf == NULL || widget->buf_w != w || widget->buf_h != h;

    if(!resized && !restart) return;

    if(widget->lottie) {
        lv_obj_delete(widget->lottie);
        widget->lottie = NULL;
    }

    if(widget->asset == NULL || w <= 0 || h <= 0) return;

    if(resized) {
        lv_free(widget->buf);
        /* lv_lottie_set_buffer() aligns the start, so leave room for that */
        widget->buf = lv_malloc((size_t)w * h * 4 + LV_DRAW_BUF_ALIGN);
        if(widget->buf == NULL) {
            LV_LOG_WARN("no memory for a %" LV_PRId32 "x%" LV_PRId32 " Lottie buffer", w, h);
            return;
        }
        widget->buf_w = w;
        widget->buf_h = h;
    }

    widget->lottie = lv_lottie_create(obj);
    lv_obj_set_size(widget->lottie, w, h);
    lv_lottie_set_buffer(widget->lottie, w, h, widget->buf);
    lv_lottie_set_src_data(widget->lottie, widget->asset->data, widget->asset->size);

    lv_anim_t * a = lv_lottie_get_anim(widget->lottie);
    if(a) {
        lv_anim_set_repeat_count(a, widget->loop ? LV_ANIM_REPEAT_INFINITE : 1);
        lv_anim_set_user_data(a, widget);
        lv_anim_set_completed_cb(a, anim_completed_cb);
    }
}

static void anim_completed_cb(lv_anim_t * a)
{
    lottie_player_t * widget = (lottie_player_t *)lv_anim_get_user_data(a);

    if(widget->ready_cb) widget->ready_cb((lv_obj_t *)widget, widget->ready_user_data);
}
