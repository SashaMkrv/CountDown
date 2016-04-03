#include <pebble_worker.h>
#define DEAD 0
#define CURRENT 1
static void tick_handler(struct tm *tick_timer, TimeUnits units_changed){
  uint32_t prev = persist_read_int(persist_read_int(CURRENT));
  uint32_t next;
  uint32_t morn = persist_read_int(2);
  bool night = false;
  if (persist_exists(persist_read_int(CURRENT) + 1)){
    next = persist_read_int(persist_read_int(CURRENT)+1);
  }
  else{
    next = morn;
    night = true;
  }
  uint32_t total_curr = tick_timer->tm_min + tick_timer->tm_hour*60;
  if (total_curr >= next || (total_curr < prev && !(night))){
    night ? persist_write_int(CURRENT, 2): persist_write_int(CURRENT, persist_read_int(CURRENT) + 1);
  }
}

static void worker_init() {
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  persist_write_bool(DEAD, 1);
  /*uint32_t prev = persist_read_int(persist_read_int(CURRENT));
  uint32_t next;
  uint32_t morn = persist_read_int(2);
  bool night = false;
  if (persist_exists(persist_read_int(CURRENT) + 1)){
    next = persist_read_int(persist_read_int(CURRENT)+1);
  }
  else{
    next = morn;
    night = true;
  }*/
}

static void worker_deinit() {
  tick_timer_service_unsubscribe();
  persist_write_bool(DEAD, 1);
}

int main(void){
  worker_init();
  worker_event_loop();
  worker_deinit();
}