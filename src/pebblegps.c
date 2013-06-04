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

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, APP_NAME);
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
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
