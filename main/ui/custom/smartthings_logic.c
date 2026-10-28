/**
 * @file smartthings_logic.c
 *
 * The cards of screens/screen_main.xml are named page_0 .. page_4 and are found
 * by name on the active screen, so this also works in the Editor's preview,
 * which creates the screens itself.
 */

/*********************
 *      INCLUDES
 *********************/

#include "smartthings_logic.h"
#include "../smartthings_ui.h"
#include "../widgets/lottie_player/lottie_player.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_obj_t * find_page(int32_t page);
static bool page_available(int32_t page);
static void slide(lv_obj_t * card, int32_t from_x, int32_t to_x, lv_anim_completed_cb_t completed_cb);
static void slide_completed_cb(lv_anim_t * a);
static void page_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void device_on_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void tv_anim_ready_cb(lv_obj_t * tv_anim, void * user_data);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_subject_t * const slot_type[UI_DEVICE_SLOTS] = {
    &subject_device0_type, &subject_device1_type, &subject_device2_type
};

static lv_subject_t * const slot_on[UI_DEVICE_SLOTS] = {
    &subject_device0_on, &subject_device1_on, &subject_device2_on
};

static lv_subject_t * const slot_busy[UI_DEVICE_SLOTS] = {
    &subject_device0_busy, &subject_device1_busy, &subject_device2_busy
};

/* The page whose card is at CARD_X, which lags subject_page while sliding */
static int32_t shown_page;
static bool sliding;

/* The last on state of each slot, -1 before the first notification */
static int32_t last_on[UI_DEVICE_SLOTS] = { -1, -1, -1 };

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void smartthings_logic_init(void)
{
    shown_page = lv_subject_get_int(&subject_page);
    lv_subject_add_observer(&subject_page, page_observer_cb, NULL);

    for(int32_t i = 0; i < UI_DEVICE_SLOTS; i++) {
        lv_subject_add_observer(slot_on[i], device_on_observer_cb, (void *)(intptr_t)i);
    }
}

void ui_show_main_screen(void)
{
    /* Deletes the loading screen, as the original lv_obj_clean() did */
    lv_screen_load_anim(screen_main_create(), LV_SCREEN_LOAD_ANIM_NONE, 0, 0, true);

    /* The XML parks every card but page 0 off-screen */
    for(int32_t page = 0; page < UI_PAGE_COUNT; page++) {
        lv_obj_t * card = find_page(page);
        if(card) lv_obj_set_x(card, page == shown_page ? CARD_X : CARD_X_NEXT);
    }
}

void ui_next_page(void)
{
    if(sliding) return;

    int32_t page = lv_subject_get_int(&subject_page);
    for(int32_t i = 1; i <= UI_PAGE_COUNT; i++) {
        int32_t next = (page + i) % UI_PAGE_COUNT;
        if(page_available(next)) {
            lv_subject_set_int(&subject_page, next);
            return;
        }
    }
}

int32_t ui_current_device(void)
{
    int32_t page = lv_subject_get_int(&subject_page);
    return page < UI_DEVICE_SLOTS && page_available(page) ? page : -1;
}

void ui_toggle_current_device(void)
{
    int32_t slot = ui_current_device();
    if(slot < 0 || lv_subject_get_int(slot_busy[slot]) || !ui_devices_has_api()) return;

    /* The backend clears it when the device has answered */
    lv_subject_set_int(slot_busy[slot], 1);
    ui_devices_set_power(slot, !lv_subject_get_int(slot_on[slot]));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * find_page(int32_t page)
{
    char name[16];
    lv_snprintf(name, sizeof(name), "page_%" LV_PRId32, page);
    return lv_obj_find_by_name(lv_screen_active(), name);
}

static bool page_available(int32_t page)
{
    if(page >= UI_DEVICE_SLOTS) return true;
    return lv_subject_get_int(slot_type[page]) != REF_DEVICE_NONE;
}

static void slide(lv_obj_t * card, int32_t from_x, int32_t to_x, lv_anim_completed_cb_t completed_cb)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, card);
    lv_anim_set_values(&a, from_x, to_x);
    lv_anim_set_duration(&a, UI_SLIDE_TIME);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_completed_cb(&a, completed_cb);
    lv_anim_start(&a);
}

static void slide_completed_cb(lv_anim_t * a)
{
    LV_UNUSED(a);
    sliding = false;
}

/* The current card leaves to the left while the new one comes in from the right */
static void page_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);

    int32_t page = lv_subject_get_int(subject);
    if(page == shown_page) return;

    lv_obj_t * card_out = find_page(shown_page);
    lv_obj_t * card_in = find_page(page);
    shown_page = page;

    /* Not on the main screen: ui_show_main_screen() places the cards */
    if(card_in == NULL) return;

    if(card_out) slide(card_out, CARD_X, CARD_X_PREV, NULL);
    slide(card_in, CARD_X_NEXT, CARD_X, slide_completed_cb);
    sliding = true;
}

/* A TV in view plays its turn-on or turn-off animation over the static icons
 * when its state changes, then the icons bound to its on subject come back */
static void device_on_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    int32_t slot = (int32_t)(intptr_t)lv_observer_get_user_data(observer);
    int32_t on = lv_subject_get_int(subject);
    int32_t was_on = last_on[slot];
    last_on[slot] = on;

    if(was_on < 0 || was_on == on) return;
    if(lv_subject_get_int(slot_type[slot]) != REF_DEVICE_TV) return;
    if(lv_subject_get_int(&subject_page) != slot) return;

    lv_obj_t * card = find_page(slot);
    if(card == NULL) return;
    lv_obj_t * tv_static = lv_obj_find_by_name(card, "tv_static");
    lv_obj_t * tv_anim = lv_obj_find_by_name(card, "tv_anim");
    if(tv_static == NULL || tv_anim == NULL) return;

    lv_obj_add_flag(tv_static, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(tv_anim, LV_OBJ_FLAG_HIDDEN);
    lottie_player_set_ready_cb(tv_anim, tv_anim_ready_cb, tv_static);
    lottie_player_set_src(tv_anim, on ? "tv_turn_on" : "tv_turn_off");
}

static void tv_anim_ready_cb(lv_obj_t * tv_anim, void * user_data)
{
    lv_obj_add_flag(tv_anim, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag((lv_obj_t *)user_data, LV_OBJ_FLAG_HIDDEN);
}
