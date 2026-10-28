# SmartThings UI (LVGL Pro)

The UI of the LVGL ESP32 SmartThings Controller for the LILYGO T-Display S3
(320x170), as an LVGL Pro project. The firmware builds it from `main/ui/`. It shows a
carousel of Samsung SmartThings devices (air conditioners and TVs) with Lottie
animations, then the average temperature and humidity, with one page dot per
card. The board's single button pages through the cards and toggles the device
in view.

Only the UI is here. Wi-Fi, the SmartThings API, the button driver and the
display driver stay in the firmware. State goes from the firmware to the UI
through subjects; commands go from the UI to the firmware through a registered
callback API, `ui_devices`.

## Layout

```
project.xml            target t_display_s3, 320x170, LVGL 9.5.0
globals.xml            palette, geometry, ref_* enumerations, fonts, subjects, images, styles
screens/
  screen_loading.xml   logo and spinner, shown until the devices are fetched
  screen_main.xml      header, the five cards (page_0 .. page_4), page dots
components/
  device_card/         a device slot: AC or TV icon, power button, name, status
  sensor_card/         temperature / humidity card
  page_dot/            one dot of the page indicator
widgets/
  lottie_player/       plays a compiled-in Lottie animation (XML has no lv_lottie yet)
custom/
  smartthings_logic.[ch]   page carousel, TV on/off transition, application API
  ui_devices/              the commands the UI sends, dispatched to a registered backend
  app_simulator.[ch]       the firmware's part (a ui_devices backend), in the Editor's preview only
  lottie_assets/           anim_*.json, their C arrays, lookup by name
scripts/lottie_to_c.py     regenerates custom/lottie_assets/anim_*.c
smartthings_ui.c           init: generated init, logic, simulator, loading screen
tests/test_button.xml      paging and toggling through the simulated button
fonts/  images/            Montserrat TTFs and the PNG assets of the original UI
```

Generated files (`*_gen.c`, `*_gen.h`, `fonts/*_data.c`, `images/*_data.c`,
`file_list_gen.cmake`) come from the XML: edit the XML and regenerate.

## Using it in an application

Initialize the UI once LVGL and the display are up. It creates and loads the
loading screen:

```c
#include "smartthings_ui.h"

smartthings_ui_init("");
```

`smartthings_ui.h` is the only header an application needs.

The main screen has three **device slots** and two sensor cards. A slot shows
one device; a slot whose type is `REF_DEVICE_NONE` is hidden, loses its page dot
and is skipped while paging. The original UI also topped out at five page dots,
i.e. three devices.

| Subject                       | Type   | Meaning                                                    |
| ----------------------------- | ------ | ---------------------------------------------------------- |
| `subject_device<N>_type`      | int    | `REF_DEVICE_NONE` / `_TV` / `_AC`; map firmware types to these, see below |
| `subject_device<N>_name`      | string | device label                                               |
| `subject_device<N>_detail`    | string | status shown while on, e.g. `"Current 23°C"`, the TV channel |
| `subject_device<N>_on`        | int    | 1 when on; a TV in view animates the change                |
| `subject_device<N>_busy`      | int    | 1 while a toggle is in flight: spinner instead of the power button |
| `subject_temperature`         | float  | average AC temperature, °C                                 |
| `subject_humidity`            | float  | average AC humidity, %                                     |
| `subject_page`                | int    | page in view: slot 0..2, `REF_PAGE_TEMPERATURE`, `REF_PAGE_HUMIDITY` |

| Function                     | Replaces (original `ui_smartthings.c`) |
| ---------------------------- | -------------------------------------- |
| `ui_show_main_screen()`      | `ui_switch_to_main_screen()`           |
| `ui_next_page()`             | `on_button_pressed()`                  |
| `ui_toggle_current_device()` | `on_button_hold()`                     |
| `ui_current_device()`        | the slot in view; -1 on a sensor card  |

`ui_set_devices()` and `update_device_ui()` become subject writes.

Call all of these from the thread that runs LVGL, or with the LVGL lock held.

### Commands: the `ui_devices` API

Switching a device on or off has to reach the SmartThings API, which the UI must
not know about. The UI only calls `ui_devices_set_power()`
(`custom/ui_devices/ui_devices_api.h`); whoever controls the devices registers
what that does:

```c
static void set_power(int32_t slot, bool on, void * user_data)
{
    /* Start the request without blocking. When the device has answered, with
     * the LVGL lock held: set subject_device<slot>_on to the new state if it
     * worked, then subject_device<slot>_busy to 0. */
}

static const ui_devices_api_t api = { .set_power = set_power };
ui_devices_register_api(&api, &my_context); /* my_context comes back as user_data */
```

`ui_toggle_current_device()` sets the slot's busy subject to 1 (the spinner) and
calls `set_power` with the opposite of its on subject. Without a registered
backend it does nothing. On the board the backend is the firmware's
`main/app/app_ui.c`; in the Editor's preview it is `custom/app_simulator.c`.

### Firmware side

`main/app/app_ui.c` in the firmware is the whole glue:

- `app_ui_init()` calls `smartthings_ui_init()` and registers the `ui_devices`
  backend, whose `set_power` runs `smartthings_api_POST_device_status()` in a
  task.
- `app_ui_show_devices()` writes the fetched devices into the slot subjects and
  the AC averages into the sensor subjects, then calls `ui_show_main_screen()`.
- `app_ui_button_pressed()` and `app_ui_button_hold()` take the LVGL lock and
  call `ui_next_page()` and `ui_toggle_current_device()`.

The device type values belong to the UI (`ref_device_*` in `globals.xml`, which
generate `REF_DEVICE_*`). The firmware keeps its own `smartthings_device_type_t`
and translates in the glue, so renumbering either enum breaks nothing, and a
device type the UI doesn't know never gets a slot. Only the glue includes both
headers; the API code doesn't depend on the UI.

```c
static int32_t ui_device_type(smartthings_device_type_t type)
{
    switch (type) {
        case DEVICE_TYPE_TV: return REF_DEVICE_TV;
        case DEVICE_TYPE_AC: return REF_DEVICE_AC;
        default:             return REF_DEVICE_NONE;
    }
}
```

## Building with the application

- Compile the sources listed in `file_list_gen.cmake` and in `user_config.cmake`
  (`LV_EDITOR_PROJECT_SOURCES`), with this directory on the include path. Define
  `LV_LVGL_H_INCLUDE_SIMPLE` if `lvgl.h` is on the include path directly rather
  than as `lvgl/lvgl.h`.
- **LVGL 9.5.0**, the version `project.xml` targets. LVGL Pro doesn't support
  9.6 yet.
- `lv_conf.h` needs `LV_USE_LOTTIE`, `LV_USE_THORVG_INTERNAL`,
  `LV_USE_VECTOR_GRAPHIC`, `LV_USE_CANVAS`, `LV_USE_MATRIX`, `LV_USE_FLOAT` (float
  subjects), `LV_USE_OBJ_NAME` (the generated code names objects and
  `custom/smartthings_logic.c` finds the cards by name) and `LV_USE_OBSERVER`.
  `%.1f` must work in `lv_snprintf`: LVGL 9.5's builtin `lv_snprintf` formats
  floats when `LV_USE_FLOAT` is on. `LV_USE_XML` is not needed.
- ThorVG is C++, so the build needs a C++ compiler (ESP-IDF has one).
- `custom/app_simulator.c` is in the source list but compiles to nothing
  without `LV_EDITOR_PREVIEW`.
- Each `lottie_player` allocates a `width x height x 4` byte render buffer with
  `lv_malloc()`: about 60 kB on the main screen (two AC animations, three power
  button spinners, one TV transition) and 30 kB for the loading spinner, which
  is freed with its screen. That doesn't fit LVGL's default 64 kB builtin heap:
  the firmware uses `LV_STDLIB_CLIB` for `LV_USE_STDLIB_MALLOC`, so these go to
  the system heap, which includes the board's PSRAM. As in the original UI,
  hidden animations keep rendering their frames.
- The display of the board is RGB565; the UI was checked rendering to RGB565.

## Preview and the app simulator

In the Editor's preview `custom/app_simulator.c` plays the firmware's part:

- a **click** anywhere is a short press of the board's button: next page
- a **long press** calls `ui_toggle_current_device()`. The simulator is the
  registered `ui_devices` backend: spinner for 1.2 s (the simulated API call),
  then the new state, with the TV's animation
- every 5 s the air conditioners report slightly different readings, and the
  sensor cards show their average, as `get_average_ac_metrics()` did

The preview opens each screen directly, so it does not go from the loading
screen to the main screen by itself. The subject defaults in `globals.xml` are
the devices of the original repo's screenshots.

## Lottie animations

`widgets/lottie_player` is a custom widget hosting an `lv_lottie`, since LVGL Pro
XML has no Lottie tag yet. It works in the preview too: the preview runtime is
built with Lottie and ThorVG, and it runs this project's C code.

```xml
<lottie_player src="loading" width="86" height="86" />
<lottie_player name="tv_anim" width="44" height="38" loop="false" />
```

`src` names an animation from `custom/lottie_assets/lottie_assets.c`. To add one,
drop the JSON in `custom/lottie_assets/`, run `python3 scripts/lottie_to_c.py`,
add it to the table in `lottie_assets.c`, and add its `.c` to `user_config.cmake`.

From C, `lottie_player_play()` restarts the animation and
`lottie_player_set_ready_cb()` reports the end of a non-looping one.

## Fonts

The `<bin>` fonts in `globals.xml` are rendered from the TTF files in `fonts/` at
4 bpp, for printable ASCII plus the degree sign: the same sizes and range as the
original `lv_font_conv` fonts.

| File                      | Font       | License     |
| ------------------------- | ---------- | ----------- |
| `Montserrat-Bold.ttf`     | Montserrat | SIL OFL 1.1 |
| `Montserrat-SemiBold.ttf` | Montserrat | SIL OFL 1.1 |

## Regenerating and testing

With the LVGL Pro CLI and a license token:

```
lvglpro validate   .
lvglpro generate   .
lvglpro compile    . --target editor
lvglpro screenshot . screens/screen_main.xml --out main.png --delay 500
lvglpro run-test   . tests/test_button.xml
```

With CLI 2.0.5, `screenshot` and `run-test` keep using the
`preview-bin/lved-runtime.node.*` they built first. After changing C code or
subject defaults, delete those two files so they are rebuilt.

## How it was converted

The XML started from `lvgl_app2pro` (`tools/lvgl_app2pro` in the `lvgl_pro`
repo, `dev` branch). It runs an LVGL app on the PC under GDB and dumps its live
widget tree. `main/ui` of the firmware was built for the PC against LVGL 9.5 with
stubs for ESP-IDF and FreeRTOS and the screenshot devices as data, and both
screens were dumped from one stop. The positions, sizes, colors and images came
from that dump, and the converted screens matched the original pixel for pixel.

Done by hand afterwards: the fonts (the converter cannot recover them), the
`lv_lottie` objects (it writes them as plain `lv_obj`), the device and sensor
cards as components, the data as subjects in place of `ui_set_devices()`, the
names of the consts, and the behavior in `custom/`.
