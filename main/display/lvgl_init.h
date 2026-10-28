#ifndef _LVGL_INIT_H_
#define _LVGL_INIT_H_

#define LVGL_TICK_PERIOD_MS 1

void lvgl_init(void);
void create_lvgl_task(void);

// LVGL isn't thread safe: other tasks hold this lock while they call into LVGL
// or the UI. The LVGL task holds it while it runs lv_timer_handler(), so LVGL's
// own callbacks already have it.
void lvgl_port_lock(void);
void lvgl_port_unlock(void);

#endif // _LVGL_INIT_H_
