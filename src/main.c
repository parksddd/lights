#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
InverterLayer *inv_layer;
char response_buffer[64];
enum {
  CMD = 0,
  RESPONSE = 1
};

void send_cmd(char cmd_text[32]){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletCString(CMD, cmd_text);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_cmd("/next");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_cmd("/up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_cmd("/down");
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_cmd("/4");
}

void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "High");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

void process_tuple(Tuple *t){
  int key = t->key;
  char string_value[64];
  strcpy(string_value, t->value->cstring);
  switch(key){
    case RESPONSE:
      snprintf(response_buffer, sizeof("Some Text Goes Here"), "%s", string_value);
      text_layer_set_text(text_layer, (char*) &response_buffer);
      break;
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context){
  (void) context;
  Tuple *t = dict_read_first(iter);
  while (t != NULL){
    process_tuple(t);
    t = dict_read_next(iter);
  }
}

static void window_load(Window *window) {
  text_layer = text_layer_create(GRect(0,53,132,168));
  text_layer_set_background_color(text_layer,GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_get_root_layer(window),text_layer_get_layer(text_layer));
  inv_layer = inverter_layer_create(GRect(0,0,144,168));
  layer_add_child(window_get_root_layer(window),(Layer*) inv_layer);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  inverter_layer_destroy(inv_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  app_message_register_inbox_received(in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
