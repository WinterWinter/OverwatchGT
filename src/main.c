#include <pebble.h>

#define KEY_SCALE 1
#define KEY_SCALE_OPTION 2
#define KEY_TEMPERATURE 3
#define KEY_VIBRATE 4
#define KEY_HERO 5


static Window *s_main_window;
static TextLayer *s_time_layer;

static bool initiate_watchface = true;

static TextLayer *day_layer;
static TextLayer *temperature_layer;
static TextLayer *high_noon_layer;


static GFont s_time_font;
static GFont all_font;

int vibrate = 1;

int hero_choice = 21;
int HEROES = 21;
int seed_mugs2;
int start_number_mug, random_mug;

#define TOTAL_HERO 1
static GBitmap *hero_images[TOTAL_HERO];
static BitmapLayer *hero_layers[TOTAL_HERO];

const int hero[] = {
    RESOURCE_ID_Bastion,
    RESOURCE_ID_Dva,
    RESOURCE_ID_Genji,
    RESOURCE_ID_Hanzo,
    RESOURCE_ID_Junkrat,
    RESOURCE_ID_Lucio,
    RESOURCE_ID_McCree,
    RESOURCE_ID_Mei,
    RESOURCE_ID_Mercy,
    RESOURCE_ID_Pharah,
    RESOURCE_ID_Reaper,
    RESOURCE_ID_Reinhardt,
    RESOURCE_ID_Roadhog,
    RESOURCE_ID_Soldier76,
    RESOURCE_ID_Symmetra,
    RESOURCE_ID_Torbjorn,
    RESOURCE_ID_Tracer,
    RESOURCE_ID_Widowmaker,
    RESOURCE_ID_Winston,
    RESOURCE_ID_Zarya,
    RESOURCE_ID_Zenyatta
};

#define TOTAL_UI 1
static GBitmap *ui_images[TOTAL_UI];
static BitmapLayer *ui_layers[TOTAL_UI];

const int ui[] = {
    RESOURCE_ID_Morning_UI,
    RESOURCE_ID_Day_UI,
    RESOURCE_ID_Evening_UI,
    RESOURCE_ID_Night_UI
};

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) 
{
GBitmap *old_image = *bmp_image;

 	*bmp_image = gbitmap_create_with_resource(resource_id);
 	GRect frame = (GRect) {
   	.origin = origin,
    .size = gbitmap_get_bounds(*bmp_image).size 
};
 	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
 	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

 	if (old_image != NULL) {
 	gbitmap_destroy(old_image);
  }
}

static void handle_bluetooth(bool connected) 
{	
if (connected) {

		if (!initiate_watchface) {//watch becomes connected after watchface is already loaded
			vibes_double_pulse();
      DictionaryIterator *iter;
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, 0, 0);
      app_message_outbox_send();
		}
	}
	else {

		if (!initiate_watchface) {//becomes disconnected while watchface is already loaded     
			vibes_enqueue_custom_pattern( (VibePattern) {
   				.durations = (uint32_t []) {100, 100, 100, 100, 100},
   				.num_segments = 5
			} );
		}	
	}
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  static char day_text[] = "ABC 00";
  char *day_format;
  day_format = "%b %e";

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  strftime(day_text, sizeof(day_text), day_format, tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(day_layer, day_text);
}

static void update_display(struct tm *current_time) {
  
 	  if( ( (current_time->tm_hour != 12) && (current_time->tm_min == 0) && (current_time->tm_sec == 0)) || (initiate_watchface == true) ){

      if (initiate_watchface){
      start_number_mug = (current_time->tm_sec) + (current_time->tm_min) + HEROES;
      }
    
      static long seed_mugs = 100;
      seed_mugs  = (((seed_mugs * 214013L + 2531011L) >> 16) & 32767);
      seed_mugs2 = seed_mugs + start_number_mug;
      random_mug = (seed_mugs2 % HEROES);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "random character generated [#%d].", random_mug);
      set_container_image(&hero_images[0], hero_layers[0], hero[random_mug], GPoint( PBL_IF_ROUND_ELSE(18, 0), PBL_IF_ROUND_ELSE(6, 0)));
    }
  
  if( ( (current_time->tm_hour == 12) && (current_time->tm_min == 0) && (current_time->tm_sec == 0) ) ){
    layer_set_hidden(text_layer_get_layer(high_noon_layer), false);
    layer_set_hidden(text_layer_get_layer(s_time_layer), true);
    set_container_image(&hero_images[0], hero_layers[0], hero[6], GPoint( PBL_IF_ROUND_ELSE(18, 0), PBL_IF_ROUND_ELSE(6, 0)));
  }
  else{
    layer_set_hidden(text_layer_get_layer(high_noon_layer), true);
    layer_set_hidden(text_layer_get_layer(s_time_layer), false);
  }
}

static void update_bg(struct tm *current_time) {
  
  if (current_time->tm_hour >= 5 && current_time->tm_hour < 12){
    //Morning
    set_container_image(&ui_images[0], ui_layers[0], ui[0], GPoint(0,0));
  }
  else if (current_time->tm_hour >= 12 && current_time->tm_hour < 17){
    //Day
    set_container_image(&ui_images[0], ui_layers[0], ui[1], GPoint(0,0));
  }
  else if (current_time->tm_hour >= 17 && current_time->tm_hour < 21){
    //Evening
    set_container_image(&ui_images[0], ui_layers[0], ui[2], GPoint(0,0));
  } 
  else if (current_time->tm_hour >= 21 || current_time->tm_hour < 5){
    //Night
    set_container_image(&ui_images[0], ui_layers[0], ui[3], GPoint(0,0));
  }
}

static void hourly_vibrate(struct tm *current_time){
  vibrate = persist_read_int(KEY_VIBRATE);
    
  if(vibrate == 1 && (current_time->tm_min == 0) && (current_time->tm_sec == 0)){
      vibes_short_pulse();
  }
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  if(hero_choice == 21){
    update_display(tick_time);
    }
  update_bg(tick_time);
  hourly_vibrate(tick_time);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) 
{
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];
  
  Tuple *t = dict_read_first(iterator);

  int temperature;
  int finalTemp;
  int scale_option = persist_read_int(KEY_SCALE_OPTION);
  
  
  while(t != NULL) {
    switch(t->key) {
      
    case KEY_SCALE:
      if(strcmp(t->value->cstring, "F") == 0){
        persist_write_int(KEY_SCALE_OPTION, 0);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      else if(strcmp(t->value->cstring, "C") == 0){
        persist_write_int(KEY_SCALE_OPTION, 1);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      break;
    case KEY_TEMPERATURE:
      
      if(scale_option == 0){
      temperature = t->value->int32;
      finalTemp = (temperature - 273.15) * 1.8 + 32;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      else if(scale_option == 1){
      temperature = t->value->int32;
      finalTemp = temperature - 273.15;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      break;
      
      case KEY_VIBRATE:
      
      vibrate = atoi( t->value->cstring );
		  //APP_LOG(APP_LOG_LEVEL_INFO, "Vibrate is %d", vibrate);
		  persist_write_int(KEY_VIBRATE, vibrate); 
      break;
      
      case KEY_HERO:
      
      hero_choice = atoi( t->value->cstring );
      persist_write_int(KEY_HERO, hero_choice);
      
      //APP_LOG(APP_LOG_LEVEL_INFO, "Hero: %d", hero_choice);
      
      if(hero_choice < 21){
      set_container_image(&hero_images[0], hero_layers[0], hero[hero_choice], GPoint( PBL_IF_ROUND_ELSE(18, 0), PBL_IF_ROUND_ELSE(6, 0)));
      }
      
      break;
  }

    t = dict_read_next(iterator);
  }
  
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(temperature_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  
  // Day
  day_layer = text_layer_create(GRect( PBL_IF_ROUND_ELSE(25, 0), PBL_IF_ROUND_ELSE(138, 137), 72, 25));
  text_layer_set_background_color(day_layer, GColorClear);
  text_layer_set_text_color(day_layer, GColorWhite);
  text_layer_set_text(day_layer, "ABC 01");
  text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);
  
  // Temperature
  temperature_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(84, 72), PBL_IF_ROUND_ELSE(138, 137), 72, 25));
  text_layer_set_background_color(temperature_layer, GColorClear);
  text_layer_set_text_color(temperature_layer, GColorWhite);
  text_layer_set_text(temperature_layer, "----");
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  
  // High Noon
  high_noon_layer = text_layer_create(GRect(0,  PBL_IF_ROUND_ELSE(98, 94), PBL_IF_ROUND_ELSE(180, 144), 50));
  text_layer_set_background_color(high_noon_layer, GColorClear);
  text_layer_set_text_color(high_noon_layer, GColorWhite);
  text_layer_set_text(high_noon_layer, "HIGH NOON");
  text_layer_set_text_alignment(high_noon_layer, GTextAlignmentCenter);
  layer_set_hidden(text_layer_get_layer(high_noon_layer), true);
  
  // Time
  s_time_layer = text_layer_create(GRect(0,  PBL_IF_ROUND_ELSE(98, 94), PBL_IF_ROUND_ELSE(180, 144), 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  GRect dummy_frame = { {0, 0}, {0, 0} };
  
  for (int i = 0; i < TOTAL_UI; ++i) {
   	ui_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(ui_layers[i]));
    bitmap_layer_set_compositing_mode(ui_layers[i], GCompOpSet);
  }
  
  for (int i = 0; i < TOTAL_HERO; ++i) {
   	hero_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(hero_layers[i]));
    bitmap_layer_set_compositing_mode(hero_layers[i], GCompOpSet);
  }  
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TIME_40));
  all_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(high_noon_layer, s_time_font);
  text_layer_set_font(day_layer, all_font);
  text_layer_set_font(temperature_layer, all_font);
  
  layer_add_child(window_layer, text_layer_get_layer(day_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(high_noon_layer));
  layer_add_child(window_layer, text_layer_get_layer(temperature_layer));

}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(day_layer);
  text_layer_destroy(temperature_layer);
  text_layer_destroy(high_noon_layer);

  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  for (int i = 0; i < TOTAL_HERO; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(hero_layers[i]));
   	gbitmap_destroy(hero_images[i]);
   	bitmap_layer_destroy(hero_layers[i]); 
  }
  
  for (int i = 0; i < TOTAL_UI; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(ui_layers[i]));
   	gbitmap_destroy(ui_images[i]);
   	bitmap_layer_destroy(ui_layers[i]); 
  }

}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();
  
  time_t now = time(NULL);
 	struct tm *tick_time = localtime(&now);
  
  hero_choice = persist_read_int(KEY_HERO);
  
  if(hero_choice == 21){
    update_display(tick_time);
    }
    else{
      set_container_image(&hero_images[0], hero_layers[0], hero[hero_choice], GPoint( PBL_IF_ROUND_ELSE(18, 0), PBL_IF_ROUND_ELSE(6, 0)));
    }
  
  update_bg(tick_time);
  initiate_watchface = false;
  
  handle_bluetooth(bluetooth_connection_service_peek());
  bluetooth_connection_service_subscribe(&handle_bluetooth);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(128,128); 
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  persist_write_int(KEY_HERO, hero_choice);
  bluetooth_connection_service_unsubscribe();
  bluetooth_connection_service_peek();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}