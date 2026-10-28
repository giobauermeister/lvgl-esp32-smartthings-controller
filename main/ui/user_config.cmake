# =============================================================================
# user_config.cmake - Add your custom source files here
# =============================================================================
#
# This file is included by the generated CMakeLists.txt and allows you to
# add extra source files to the project without modifying generated files
# (which may be overwritten).
#
# To add your own sources, append them to LV_EDITOR_PROJECT_SOURCES:
#
#   list(APPEND LV_EDITOR_PROJECT_SOURCES
#       ${CMAKE_CURRENT_LIST_DIR}/src/my_widget.c
#       ${CMAKE_CURRENT_LIST_DIR}/src/my_screen.c
#   )
#
# Tip:
#   - Use ${CMAKE_CURRENT_LIST_DIR} to get paths relative to this file
#
# =============================================================================

list(APPEND LV_EDITOR_PROJECT_SOURCES
    # The page carousel, the TV transition and the application API.
    # See custom/smartthings_logic.h.
    ${CMAKE_CURRENT_LIST_DIR}/custom/smartthings_logic.c

    # The commands the UI sends to the backend, which registers its callbacks.
    # See custom/ui_devices/ui_devices_api.h.
    ${CMAKE_CURRENT_LIST_DIR}/custom/ui_devices/ui_devices_api.c

    # The firmware's part, simulated in the Editor's preview only
    # (LV_EDITOR_PREVIEW); it compiles to nothing in the application.
    ${CMAKE_CURRENT_LIST_DIR}/custom/app_simulator.c

    # The Lottie animations played by widgets/lottie_player, as C arrays
    # generated from the JSON files next to them by scripts/lottie_to_c.py.
    ${CMAKE_CURRENT_LIST_DIR}/custom/lottie_assets/lottie_assets.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/lottie_assets/anim_ac_on.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/lottie_assets/anim_btn_loading.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/lottie_assets/anim_loading.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/lottie_assets/anim_tv_turn_off.c
    ${CMAKE_CURRENT_LIST_DIR}/custom/lottie_assets/anim_tv_turn_on.c
)
