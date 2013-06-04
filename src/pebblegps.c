/* vim: sw=2 ts=2
 * richo@psych0tik.net */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define APP_NAME "pebblegps"

#define MY_UUID {0x63, 0x98, 0x81, 0x4C, 0xC9, 0x97, 0x47, 0x5A, 0xA2, 0x6A, 0x0D, 0xB8, 0x0A, 0xE1, 0xF1, 0xC0}
PBL_APP_INFO(MY_UUID,
            APP_NAME, "richo@psych0tik.net",
             0, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer gpsLat;
TextLayer gpsLon;

enum {
  LAT_KEY = 0x0,
  LON_KEY = 0x1,
};

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  vibes_short_pulse();
}

inline void draw_dummy_coords(void) {
  char *lat = "-37.271634",
       *lon = "144.205442";
  text_layer_set_text(&gpsLat, lat);
  text_layer_set_text(&gpsLon, lon);
}

void bottom_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  draw_dummy_coords();
}

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) bottom_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

/* inline void _init_text_layer(TextLayer *textLayer) { */
#define _init_text_layer(layer) \
  text_layer_set_text_color(layer, GColorWhite); \
  text_layer_set_background_color(layer, GColorClear); \
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, APP_NAME);
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  /*               ^  *
   *               |  * - 24
   *               |  * |
   *              168 * | 120
   *               |  * |
   *<-----144------+> * - 24
   *               v  */

  text_layer_init(&gpsLat, GRect(20, 24, 144-40 /* width */, 60 /* height */));
  text_layer_init(&gpsLon, GRect(20, 84, 144-40 /* width */, 60 /* height */));
  _init_text_layer(&gpsLat);
  _init_text_layer(&gpsLon);

  layer_add_child(&window.layer, &gpsLat.layer);
  layer_add_child(&window.layer, &gpsLon.layer);

  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 256,
        .outbound = 256,
      }
    }
  };
  app_event_loop(params, &handlers);
}
