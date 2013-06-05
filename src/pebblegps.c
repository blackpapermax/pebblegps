/* vim: sw=2 ts=2
 * richo@psych0tik.net */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "httppebble-watch/http.c"
void itoa(int value, char *sp, int radix)
{
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;
    int sign;

    sign = (radix == 10 && value < 0);
    if (sign)   v = -value;
    else    v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    if (sign)
    *sp++ = '-';
    while (tp > tmp)
    *sp++ = *--tp;
}

// https://github.com/mludvig/mini-printf
static unsigned int
mini_itoa(int value, unsigned int radix, unsigned int uppercase,
	 char *buffer, unsigned int zero_pad)
{
	char	*pbuffer = buffer;
	int	negative = 0;
	unsigned int	i, len;

	/* No support for unusual radixes. */
	if (radix > 16)
		return 0;

	if (value < 0) {
		negative = 1;
		value = -value;
	}

	/* This builds the string back to front ... */
	do {
		int digit = value % radix;
		*(pbuffer++) = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
		value /= radix;
	} while (value > 0);

	for (i = (pbuffer - buffer); i < zero_pad; i++)
		*(pbuffer++) = '0';

	if (negative)
		*(pbuffer++) = '-';

	*(pbuffer) = '\0';

	/* ... now we reverse it (could do it recursively but will
	 * conserve the stack space) */
	len = (pbuffer - buffer);
	for (i = 0; i < len / 2; i++) {
		char j = buffer[i];
		buffer[i] = buffer[len-i-1];
		buffer[len-i-1] = j;
	}

	return len;
}

#define APP_NAME "pebblegps"

#define MY_UUID {0x63, 0x98, 0x81, 0x4C, 0xC9, 0x97, 0x47, 0x5A, 0xA2, 0x6A, 0x0D, 0xB8, 0x0A, 0xE1, 0xF1, 0xC0}
PBL_APP_INFO(HTTP_UUID,
            APP_NAME, "richo@psych0tik.net",
             0, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer gpsLat;
TextLayer gpsLon;

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  http_location_request();
  vibes_short_pulse();
  text_layer_set_text(&gpsLon, "Locating..");
}

float _abs(float f) {
  return (f > 0) ? f : -f;
}

int ftoa(char* str, float f) {
  /* So much badness will happen if str isn't long enough */
  static int i;
  i= (signed int)f;
  str += mini_itoa(i, 10, 0, str, 0);
  *str = '.'; str++;
  str += mini_itoa((signed int)(_abs(f-i) * 1000000), 10, 0, str, 0);
  *str = 0;
  return 0; // TODO Actually do something useful
}

#define LOCATION_LENGTH 33
void handle_httppebble_location(float latitude, float longitude, float altitude, float accuracy, void* context) {
  static char lat_text[LOCATION_LENGTH];
  static char lon_text[LOCATION_LENGTH];
  ftoa(lat_text, latitude);
  text_layer_set_text(&gpsLat, lat_text);

  ftoa(lon_text, longitude);
  text_layer_set_text(&gpsLon, lon_text);
  vibes_short_pulse();
}

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;
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

  http_register_callbacks((HTTPCallbacks){
    .location = handle_httppebble_location,
    }, NULL);

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
