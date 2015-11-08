#include <pebble.h>
Window* window;
TextLayer* source_layer;
TextLayer* std0ut_layer;
char source[512] = "";
char* pc = source;
char std0ut[256] = "";
char* p0ut = std0ut;
uint64_t x=0;

// Interpreter
void add_command(char c) {
  if(pc-source<(int)sizeof(source)) {
    if(c=='o') {
#ifdef PBL_SDK_2
      p0ut+=snprintf(p0ut,sizeof(std0ut)+std0ut-p0ut,"%lu ",(uint32_t)x);
#else
      p0ut+=snprintf(p0ut,sizeof(std0ut)+std0ut-p0ut,"%llu ",x);
#endif
      text_layer_set_text(std0ut_layer, std0ut);
    }
    else {
      x = ((c=='s')?x:1) * x + (c=='i') - (c=='d');
      if(x == 256 || x == (uint64_t)-1) x = 0; // Weird deadfish behavior... that's why "iissso" outputs "0" while "iissiso" outputs "289"
    }
    *pc++ = c;
    *pc = 0;
    text_layer_set_text(source_layer, source);
  }
}

void reset() {
  x = *(pc = source) = *(p0ut = std0ut) = 0;
  text_layer_set_text(source_layer, source);
  text_layer_set_text(std0ut_layer, std0ut);  
}

// Pebble specific
static void click(ClickRecognizerRef recognizer, void *context) {
  ButtonId b = click_recognizer_get_button_id(recognizer);
  add_command(b==BUTTON_ID_UP?'i':b==BUTTON_ID_DOWN?'d':b==BUTTON_ID_SELECT?'s':'o');
}

static void click_select_long(ClickRecognizerRef recognizer, void *context) {
  reset();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, click);
  window_single_click_subscribe(BUTTON_ID_DOWN, click);
  window_single_click_subscribe(BUTTON_ID_SELECT, click);
  window_long_click_subscribe(BUTTON_ID_SELECT, 1000, click_select_long, NULL);
  window_single_click_subscribe(BUTTON_ID_BACK, click);
}

void window_load(Window *window) {
  window_set_click_config_provider(window, click_config_provider);
  Layer* window_root = window_get_root_layer(window);
  source_layer = text_layer_create(PBL_IF_RECT_ELSE(GRect(0, 0, 144, 110),GRect(0, 0, 180, 110)));
	text_layer_set_font(source_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(window_root, text_layer_get_layer(source_layer));
  std0ut_layer = text_layer_create(PBL_IF_RECT_ELSE(GRect(0, 110, 144, 58),GRect(0, 110, 180, 70)));
	text_layer_set_font(std0ut_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_root, text_layer_get_layer(std0ut_layer));
#ifdef PBL_ROUND
  text_layer_enable_screen_text_flow_and_paging(source_layer, 2);
  text_layer_enable_screen_text_flow_and_paging(std0ut_layer, 2);
#endif
}

void window_unload(Window *window) {
	text_layer_destroy(source_layer);
	text_layer_destroy(std0ut_layer);
}

WindowHandlers window_handlers =
{
  .load = window_load,
  .unload = window_unload,
};

void handle_init(void) {
  window = window_create();
  window_set_window_handlers(window, window_handlers);
#ifdef PBL_SDK_2
  window_set_fullscreen(window, true);
#endif
  window_stack_push(window, true /* Animated */);
}

void handle_deinit(void) {
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}