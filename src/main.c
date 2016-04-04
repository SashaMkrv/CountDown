#include <pebble.h>
#include "./main.h"

typedef struct T {
  uint8_t hours;
  uint8_t mins;
} T;

static T *startTime;
static T *endTime;

static Window *s_main_window;
static TextLayer *s_curr_time_layer;
static TextLayer *s_prev_time_layer;
static TextLayer *s_next_time_layer;

static Layer *s_time_layer;
static Layer *s_count_layer;

static uint8_t curr_min;
static uint8_t curr_hour;

static uint8_t next_min;
static uint8_t next_hour;

static uint8_t prev_min;
static uint8_t prev_hour;

static bool midnight;

//yes this is good practise
//just hard code those in
//actually the hell am i complaining about good practise for
//the rest of this sure isn't a shining paragon of idealism
static void calc_next() {
  midnight = false;
  prev_hour = next_hour;
  prev_min = next_min;
  next_min = (prev_min + 30) % 60;
  next_hour = ((prev_min == 30) ? ((next_hour + 2) % 24) : ((next_hour +1) % 24));
  if (next_hour > endTime->hours ||
      (next_hour == endTime->hours && next_min > endTime->mins)){
    midnight=true;
    next_hour = startTime->hours;
    next_min = startTime->mins;
  }
}

//can watchfaces still have like, app bits?
//probably not, that seems like it'd wind up a bad joke
//wait how do the times get changed then
//oh no
//i see the issue now
//i guess id be weirded out if the clock app on my phone needed
//me to use anything but a widget for full functionality
//but at the same time i have no idea what is supposed to be weird on pebbles
//or smartwatches for that matter
//whoa, im behind on the times again
//ugh
//who needs options i guess in this thing whose usefuleness relies entirely on being able
//to adjust it to personal preferences
//no one needs those
//hhaha
// :'(

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char s_buffer[8];
  static char s_buffer_prev[8];
  static char s_buffer_next[8];
  
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%k:%M":"%l:%M", tick_time);
  
  bool bigMin;
  if (prev_min < 10) bigMin = false;
  else bigMin = true;
  //what a mess.
  snprintf(s_buffer_prev, sizeof(s_buffer_prev), bigMin ? "%u:%u" : "%u:0%u",
          clock_is_24h_style() ? (prev_hour):((prev_hour==12 || prev_hour==0) ? 12:
                                              (prev_hour % 12)), prev_min);

  if (next_min < 10) bigMin = false;
  else bigMin = true;
  snprintf(s_buffer_next, sizeof(s_buffer_next), bigMin ? "%u:%u":"%u:0%u",
          clock_is_24h_style() ? (next_hour):((next_hour==12||next_hour==0) ? 12 :
                                              (next_hour % 12)), next_min);
  
  text_layer_set_text(s_curr_time_layer, s_buffer);
  
  text_layer_set_text(s_prev_time_layer, s_buffer_prev);
  
  text_layer_set_text(s_next_time_layer, s_buffer_next);
  
  
  
  curr_min = tick_time->tm_min;
  curr_hour = tick_time->tm_hour;

  if (curr_min == next_min && curr_hour == next_hour) calc_next();
}

//just once when the window opens
//everytime
//this does actually seem p unwise
//wtahever
static void scan_time(){
  //should probably just stick to one checking method
  //but heeyyyy
  //ive put down one "wtahever"
  //just put down some more
  int fake_time = curr_hour * 100 + curr_min;

  int fake_early_morn = startTime->hours*100 + startTime->mins;
  int fake_late_eve = endTime->hours*100 + endTime->mins;
  
  if ((fake_time < fake_early_morn)||fake_time >= fake_late_eve){
    prev_hour = endTime->hours;
    prev_min = endTime->mins;
    next_hour = startTime->hours;
    next_min = startTime->mins;
    midnight = true;
  }
  else {
    next_hour = startTime->hours;
    next_min = startTime->mins;
    int fake_time_prev;
    int fake_time_next = next_hour *100 + next_min;
    bool nogood=true;
    while(nogood){
      calc_next();
      fake_time_next = next_hour *100 + next_min;
      fake_time_prev = prev_hour *100 + prev_min;
      if ((fake_time < fake_time_next) && (fake_time >= fake_time_prev)){
        nogood = false;
      }
    }
  }
}

static void update_count(Layer *layer, GContext *ctx){
  drawCount(ctx, layer, curr_min, curr_hour,
            next_min, next_hour,
            prev_min, prev_hour,
            midnight);
  //graphics_context_set_fill_color(ctx, GColorCyan);
  //graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}
static void draw_black(Layer *layer, GContext *ctx){
  //this is the cleanest layer forever
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_count_layer = layer_create(bounds);
  
  s_time_layer = layer_create(GRect(0, 0, bounds.size.w/2, bounds.size.h));
  
  s_prev_time_layer = text_layer_create(
    GRect(20, bounds.size.h/2 - 18 - 16, bounds.size.w/2-24, 18));
  s_curr_time_layer = text_layer_create(
    GRect(0, bounds.size.h/2 - 20, bounds.size.w/2-3, 30));
  s_next_time_layer = text_layer_create(
    GRect(20, bounds.size.h/2 + 7, bounds.size.w/2-24, 18));
  
  text_layer_set_background_color(s_curr_time_layer, GColorClear);
  text_layer_set_background_color(s_next_time_layer, GColorClear);
  text_layer_set_background_color(s_prev_time_layer, GColorClear);
  
  text_layer_set_text_color(s_curr_time_layer, GColorWhite);
  text_layer_set_text_color(s_next_time_layer, GColorWhite);
  text_layer_set_text_color(s_prev_time_layer, GColorWhite);
  
  text_layer_set_text(s_curr_time_layer, "01:20");
  text_layer_set_text(s_prev_time_layer, "01:00");
  text_layer_set_text(s_next_time_layer, "02:00");
  
  text_layer_set_font(s_curr_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_font(s_prev_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_font(s_next_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18)); 

  text_layer_set_text_alignment(s_curr_time_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_prev_time_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_next_time_layer, GTextAlignmentRight);
  
  layer_add_child(window_layer, s_count_layer);
  layer_add_child(window_layer, s_time_layer);
  layer_add_child(s_time_layer, text_layer_get_layer(s_curr_time_layer));
  layer_add_child(s_time_layer, text_layer_get_layer(s_prev_time_layer));
  layer_add_child(s_time_layer, text_layer_get_layer(s_next_time_layer));
  
  window_set_background_color(s_main_window, GColorBlack);
  
  layer_set_update_proc(s_time_layer, draw_black);
  
  layer_set_update_proc(s_count_layer, update_count);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_curr_time_layer);
  text_layer_destroy(s_prev_time_layer);
  text_layer_destroy(s_next_time_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  startTime = malloc(sizeof(T));
  endTime = malloc(sizeof(T));
  *startTime = (T) {8, 30};
  *endTime = (T) {19, 0};
  //T increment = (T) {1, 30};
  
  /*uint32_t end_time = endTime->hours*100 + endTime->mins;
  uint32_t writing_time = startTime->hours*100 + startTime->mins;
  persist_write_int(2, writing_time);
  uint8_t block = 2;
  while (writing_time != end_time){
    block = block + 1;
    writing_time = writing_time + 130;
    persist_write_int(block, writing_time);
  }*/
  
  update_time();
  
  scan_time();
  update_time();
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}