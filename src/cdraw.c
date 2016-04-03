#if defined(PBL_ROUND)
#include <pebble.h>
//oh no, the bar actually looks better on the round
//oh well, obligation and all that.
#define day (60*24)

void drawCount(GContext *ctx, Layer *layer,
               uint8_t cmins, uint8_t chours,
               uint8_t nmins, uint8_t nhours,
               uint8_t pmins, uint8_t phours,
               bool midnight){
  int total_curr = cmins + chours*60;
  int total_next = nmins + nhours*60;
  int total_prev = pmins + phours*60;
  
  int now_diff;
  int then_diff;
  
  if (midnight){
    then_diff = total_next + (day - total_prev);
    if (total_curr < total_prev){
      now_diff = total_curr + (day - total_prev);
    }
    else {
      now_diff = total_curr - total_prev;
    }
  }
  else {
    then_diff = total_next - total_prev;
    now_diff = total_curr - total_prev;
  }
  
  then_diff = then_diff - 1;
  
  //GColor *back = COLOR_FALLBACK(GColorCyan, GColorWhite);
  //GColor *front = COLOR_FALLBACK(GColorYellow, GColorBlack);
  
  
  uint8_t angle = (180*now_diff)/then_diff;
  
  //i will take this creamsicle color scheme
  ///because color is difficult
  //and i really can't justify yellow and cyan with black
  GRect bigBounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorMelon, GColorWhite));
  graphics_fill_rect(ctx, bigBounds, 0, GCornerNone);
  graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorOrange, GColorBlack));
  //crap. this one does the opposite of the rect one
  //ooookay, time to switch colours
  graphics_fill_radial(ctx, bigBounds, GOvalScaleModeFitCircle, bigBounds.size.w, 0, DEG_TO_TRIGANGLE(angle));  
  
}

#endif