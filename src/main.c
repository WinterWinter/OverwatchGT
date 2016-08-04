#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static int battery_level;

static bool toggle = true;

int weather_interval = 30;
int flick = 1;
int steps;
int comp1, comp2, comp3, comp4;
time_t auto_hide;
int temperature;

static bool initiate_watchface = true;

static TextLayer *comp1_layer;
static TextLayer *comp2_layer;
static TextLayer *comp4_layer;
static TextLayer *comp3_layer;
static TextLayer *high_noon_layer;

static GFont s_time_font;
static GFont all_font;

bool vibrate = 1;

int hero_choice = PBL_IF_BW_ELSE(22,114); // Hero Choice
int HEROES = PBL_IF_BW_ELSE(22,114); // Random Generation

#define TOTAL_HERO 1
static GBitmap *hero_images[TOTAL_HERO];
static BitmapLayer *hero_layers[TOTAL_HERO];

#if defined(PBL_PLATFORM_APLITE)
 // Aplite array
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
	RESOURCE_ID_Zenyatta,
  RESOURCE_ID_Ana
};
#else
 // Everything else
const int hero[] = {
  RESOURCE_ID_Bastion,//0
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
	RESOURCE_ID_Zenyatta,//20
  RESOURCE_ID_Bastion_Dawn,
  RESOURCE_ID_Bastion_DM,
  RESOURCE_ID_Bastion_Meadow,
  RESOURCE_ID_Bastion_Overgrown,
  RESOURCE_ID_Bastion_Sky,
  RESOURCE_ID_Dva_Blueberry,
  RESOURCE_ID_Dva_Lime,
  RESOURCE_ID_Dva_Tangerine,
  RESOURCE_ID_Dva_White_Rabbit,
  RESOURCE_ID_Genji_Cinnabar,
  RESOURCE_ID_Genji_Malachite,
  RESOURCE_ID_Genji_Ochre,
  RESOURCE_ID_Hanzo_Azuki,
  RESOURCE_ID_Hanzo_Cloud,
  RESOURCE_ID_Hanzo_Dragon,
  RESOURCE_ID_Hanzo_Midori,
  RESOURCE_ID_Hanzo_Sora,
  RESOURCE_ID_Junkrat_Bleached,
  RESOURCE_ID_Junkrat_Drowned,
  RESOURCE_ID_Junkrat_Radiated,
  RESOURCE_ID_Junkrat_Rusted,
  RESOURCE_ID_Lucio_Auditiva,
  RESOURCE_ID_Lucio_Azul,
  RESOURCE_ID_Lucio_Laranja,
  RESOURCE_ID_Lucio_Roxo,
  RESOURCE_ID_Lucio_Vermelho,
  RESOURCE_ID_McCree_Lake,
  RESOURCE_ID_McCree_Sage,
  RESOURCE_ID_McCree_Wheat,
  RESOURCE_ID_McCree_WH,
  RESOURCE_ID_Mei_Chrysanthemum,
  RESOURCE_ID_Mei_Healiotrope,
  RESOURCE_ID_Mei_Jade,
  RESOURCE_ID_Mei_Persimmon,
  RESOURCE_ID_Mercy_Amber,
  RESOURCE_ID_Mercy_Celestial,
  RESOURCE_ID_Mercy_Cobalt,
  RESOURCE_ID_Mercy_Orchid,
  RESOURCE_ID_Mercy_Verdant,
  RESOURCE_ID_Pharah_Amethyst,
  RESOURCE_ID_Pharah_Emerald,
  RESOURCE_ID_Pharah_Jackal,
  RESOURCE_ID_Pharah_Titanium,
  RESOURCE_ID_Reaper_Blood,
  RESOURCE_ID_Reaper_Midnight,
  RESOURCE_ID_Reaper_Moss,
  RESOURCE_ID_Reaper_Royal,
  RESOURCE_ID_Reaper_Wight,
  RESOURCE_ID_Reinhardt_Blackhardt,
  RESOURCE_ID_Reinhardt_Cobalt,
  RESOURCE_ID_Reinhardt_Veridian,
  RESOURCE_ID_Roadhog_Kiwi,
  RESOURCE_ID_Roadhog_Stitched,
  RESOURCE_ID_Roadhog_Thistle,
  RESOURCE_ID_Soldier76_Jet,
  RESOURCE_ID_Soldier76_Olive,
  RESOURCE_ID_Soldier76_Smoke,
  RESOURCE_ID_Symmetra_Cardamom,
  RESOURCE_ID_Symmetra_Hyacinth,
  RESOURCE_ID_Symmetra_Saffron,
  RESOURCE_ID_Symmetra_Technomancer,
  RESOURCE_ID_Symmetra_Utopaea,
  RESOURCE_ID_Torbjorn_Bla,
  RESOURCE_ID_Torbjorn_Citron,
  RESOURCE_ID_Torbjorn_Gron,
  RESOURCE_ID_Torbjorn_Plommon,
  RESOURCE_ID_Tracer_EP,
  RESOURCE_ID_Tracer_HP,
  RESOURCE_ID_Tracer_NG,
  RESOURCE_ID_Tracer_Posh,
  RESOURCE_ID_Tracer_RB,
  RESOURCE_ID_Widowmaker_Ciel,
  RESOURCE_ID_Widowmaker_Rose,
  RESOURCE_ID_Widowmaker_Vert,
  RESOURCE_ID_Widowmaker_Winter,
  RESOURCE_ID_Winston_Atmosphere,
  RESOURCE_ID_Winston_Banana,
  RESOURCE_ID_Winston_Forest,
  RESOURCE_ID_Winston_RP,
  RESOURCE_ID_Zarya_Brick,
  RESOURCE_ID_Zarya_Dawn,
  RESOURCE_ID_Zarya_Goldenrod,
  RESOURCE_ID_Zarya_Taiga,
  RESOURCE_ID_Zarya_Violet,
  RESOURCE_ID_Zenyatta_Air,
  RESOURCE_ID_Zenyatta_Ascendant,
  RESOURCE_ID_Zenyatta_Leaf,
  RESOURCE_ID_Zenyatta_Water,
  RESOURCE_ID_Ana,
  RESOURCE_ID_Ana_Garnet,
  RESOURCE_ID_Ana_Merciful,
  RESOURCE_ID_Ana_Peridot,
  RESOURCE_ID_Ana_Turquoise, 
  };
#endif


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
  /*
  load_buffer = malloc_low(3500);
	size_t png_size; 
	size_t mResource_size;
	mResource_size = resource_size(resource_get_handle(RESOURCE_ID_MENU_IMAGE));
	APP_LOG(APP_LOG_LEVEL_DEBUG,"resource_size: %d", (int) mResource_size);
	png_size = resource_load(resource_get_handle(RESOURCE_ID_MENU_IMAGE),load_buffer, mResource_size);
	APP_LOG(APP_LOG_LEVEL_DEBUG,"png_size: %d", (int) png_size);
	mBitmap = gbitmap_create_from_png_data(load_buffer, png_size);
  */
  
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

static void show (){
	layer_set_hidden(text_layer_get_layer(comp3_layer), true);
	layer_set_hidden(text_layer_get_layer(comp4_layer), true);
	layer_set_hidden(text_layer_get_layer(comp1_layer), false);
	layer_set_hidden(text_layer_get_layer(comp2_layer), false);
}

static void handle_tap(AccelAxisType axis, int32_t direction)
{
	if(flick == 1){
		if(toggle){
			layer_set_hidden(text_layer_get_layer(comp3_layer), false);
			layer_set_hidden(text_layer_get_layer(comp4_layer), false);
			layer_set_hidden(text_layer_get_layer(comp1_layer), true);
			layer_set_hidden(text_layer_get_layer(comp2_layer), true);
			toggle = false;
		}
		else{
			layer_set_hidden(text_layer_get_layer(comp3_layer), true);
			layer_set_hidden(text_layer_get_layer(comp4_layer), true);
			layer_set_hidden(text_layer_get_layer(comp1_layer), false);
			layer_set_hidden(text_layer_get_layer(comp2_layer), false);
			toggle = true;
		}
	}
	else if(flick == 2){
		app_timer_register(4000, show, NULL);
		layer_set_hidden(text_layer_get_layer(comp3_layer), false);
		layer_set_hidden(text_layer_get_layer(comp4_layer), false);
		layer_set_hidden(text_layer_get_layer(comp1_layer), true);
		layer_set_hidden(text_layer_get_layer(comp2_layer), true);
	}
}

static void battery_callback(BatteryChargeState state) {
	battery_level = state.charge_percent;
	static char battery_buffer[32];
	snprintf(battery_buffer, sizeof(battery_buffer), "%d", battery_level);
  
  if (comp1 == 1){text_layer_set_text(comp1_layer, battery_buffer);}
  else if (comp2 == 1){text_layer_set_text(comp2_layer, battery_buffer);}
  else if (comp3 == 1){text_layer_set_text(comp3_layer, battery_buffer);}
  else if (comp4 == 1){text_layer_set_text(comp4_layer, battery_buffer);}
	
}

static void handle_bluetooth(bool connected)
{
	if (connected) {
		 
    if(comp1 == 2){text_layer_set_text(comp1_layer, "On");}
    else if(comp2 == 2){text_layer_set_text(comp2_layer, "On");}
    else if(comp3 == 2){text_layer_set_text(comp3_layer, "On");}
    else if(comp4 == 2){text_layer_set_text(comp4_layer, "On");}
		
		if (!initiate_watchface) {//watch becomes connected after watchface is already loaded
		}
	}
	else {
		
    if(comp1 == 2){text_layer_set_text(comp1_layer, "Off");}
    else if(comp2 == 2){text_layer_set_text(comp2_layer, "Off");}
    else if(comp3 == 2){text_layer_set_text(comp3_layer, "Off");}
    else if(comp4 == 2){text_layer_set_text(comp4_layer, "Off");}
		
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
  
  static char weekday_text[] = "ABC";
	char *weekday_format;
	weekday_format = "%a"; 
	
	// Write the current hours and minutes into a buffer
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
	"%H:%M" : "%I:%M", tick_time);
	strftime(day_text, sizeof(day_text), day_format, tick_time);
  
  strftime(weekday_text, sizeof(weekday_text), weekday_format, tick_time);
  
	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);
	
  if(comp1 == 3){text_layer_set_text(comp1_layer, day_text);}
  else if (comp2 == 3){text_layer_set_text(comp2_layer, day_text);}
  else if (comp3 == 3){text_layer_set_text(comp3_layer, day_text);}
  else if (comp4 == 3){text_layer_set_text(comp4_layer, day_text);}
  
  if(comp1 == 6){text_layer_set_text(comp1_layer, weekday_text);}
  else if (comp2 == 6){text_layer_set_text(comp2_layer, weekday_text);}
  else if (comp3 == 6){text_layer_set_text(comp3_layer, weekday_text);}
  else if (comp4 == 6){text_layer_set_text(comp4_layer, weekday_text);}
}

static void update_display(struct tm *current_time) {
	
	if( ( (current_time->tm_hour != 12) && (current_time->tm_min == 0) && (current_time->tm_sec == 0)) || (initiate_watchface == true) ){
		
    int r = rand() % HEROES;
    //APP_LOG(APP_LOG_LEVEL_INFO, "Random Number: %d", r);
    
		set_container_image(&hero_images[0], hero_layers[0], hero[r], GPoint( PBL_IF_ROUND_ELSE(18, 0), PBL_IF_ROUND_ELSE(6, 0)));
	}
  
}

static void update_bg(struct tm *current_time) {
	
	if (current_time->tm_hour >= 5 && current_time->tm_hour < 12){
    
		set_container_image(&ui_images[0], ui_layers[0], ui[0], GPoint( 0, 0));
	}
	else if (current_time->tm_hour >= 12 && current_time->tm_hour < 17){
		//Day
		set_container_image(&ui_images[0], ui_layers[0], ui[1], GPoint( 0, 0));
	}
	else if (current_time->tm_hour >= 17 && current_time->tm_hour < 21){
		//Evening
		set_container_image(&ui_images[0], ui_layers[0], ui[2], GPoint( 0, 0));
		
	}
	else if (current_time->tm_hour >= 21 || current_time->tm_hour < 5){
		//Night
		set_container_image(&ui_images[0], ui_layers[0], ui[3], GPoint( 0, 0));
	}
}

static void hourly_vibrate(struct tm *current_time){
	vibrate = persist_read_int(MESSAGE_KEY_HourlyVibe);
	
	if(vibrate == 1 && (current_time->tm_min == 0) && (current_time->tm_sec == 0)){
		vibes_short_pulse();
	}
	
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	
	if(hero_choice == 1000 && (tick_time->tm_min == 0) && (tick_time->tm_sec == 0)){
		update_display(tick_time);
	}
  
  if((tick_time->tm_min == 0) && (tick_time->tm_sec == 0)){
    hourly_vibrate(tick_time);
    update_bg(tick_time);
  }
  
  if(persist_exists(MESSAGE_KEY_Interval)){
    weather_interval = persist_read_int(MESSAGE_KEY_Interval);
  }
  else{
    weather_interval = 30;
      }
  
  if( ( (tick_time->tm_hour == 12) && (tick_time->tm_min == 0) ) ){
		layer_set_hidden(text_layer_get_layer(high_noon_layer), false);
		layer_set_hidden(text_layer_get_layer(s_time_layer), true);
	}
	else{
		layer_set_hidden(text_layer_get_layer(high_noon_layer), true);
		layer_set_hidden(text_layer_get_layer(s_time_layer), false);
	}
	
  //APP_LOG(APP_LOG_LEVEL_INFO, "Weather Interval: %d", weather_interval);
	
	if(tick_time->tm_min % weather_interval == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather updated.");
	}
	
}

static void handle_health(HealthEventType event, void *context) 
{
	time_t start = time_start_of_today();
	time_t end = time(NULL);
  
  static char step_buffer[32];
  
	HealthServiceAccessibilityMask mask = health_service_metric_accessible(HealthMetricStepCount, start, end);  
		
		if (mask & HealthServiceAccessibilityMaskAvailable) {
				//APP_LOG(APP_LOG_LEVEL_INFO, "Step data available!");
				steps = health_service_sum_today(HealthMetricStepCount);
        
        snprintf(step_buffer, sizeof(step_buffer), "%d", steps);
      
      if(comp1 == 4){text_layer_set_text(comp1_layer, step_buffer);}
      else if(comp2 == 4){text_layer_set_text(comp2_layer, step_buffer);}
      else if(comp3 == 4){text_layer_set_text(comp3_layer, step_buffer);}
      else if(comp4 == 4){text_layer_set_text(comp4_layer, step_buffer);}
        
				//APP_LOG(APP_LOG_LEVEL_INFO, "Steps: %d", steps);
		} else {
				//APP_LOG(APP_LOG_LEVEL_INFO, "Step data unavailable");
		}
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];
  
  Tuple *hero_t = dict_find(iter, MESSAGE_KEY_Hero);
  if(hero_t) {
    hero_choice = atoi( hero_t->value->cstring );
    
    persist_write_int(MESSAGE_KEY_Hero, hero_choice);
			
		//APP_LOG(APP_LOG_LEVEL_INFO, "Hero: %d", hero_choice);
			
		if(hero_choice < 1000){
			set_container_image(&hero_images[0], hero_layers[0], hero[hero_choice], GPoint( PBL_IF_ROUND_ELSE(18, 0), PBL_IF_ROUND_ELSE(6, 0)));
		}
  }
  
  Tuple *vibe_t = dict_find(iter, MESSAGE_KEY_HourlyVibe);
  if(vibe_t) {
    vibrate = vibe_t->value->int32 == 1;
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Vibe: %d", vibrate);
    
    persist_write_int(MESSAGE_KEY_HourlyVibe, vibrate);
  }
  
  Tuple *flick_t = dict_find(iter, MESSAGE_KEY_Flick);
  if(flick_t) { 
    flick = atoi( flick_t->value->cstring );
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Flick: %d", flick);
    
    persist_write_int(MESSAGE_KEY_Flick, flick);
  }
  
  Tuple *interval_t = dict_find(iter, MESSAGE_KEY_Interval);
  if(interval_t) { 
    weather_interval = atoi( interval_t->value->cstring );
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Interval Setting: %d", weather_interval);
    
    persist_write_int(MESSAGE_KEY_Interval, weather_interval);
  } 
  
  Tuple *temperature_t = dict_find(iter, MESSAGE_KEY_Temperature);
  if(temperature_t) { 
    temperature = temperature_t->value->int32;
    
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", temperature);
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Temperature: %d", temperature);
  } 
  
  Tuple *scale_t = dict_find(iter, MESSAGE_KEY_Scale);
  if(scale_t) {
    
      DictionaryIterator *iter;
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, 0, 0);
      app_message_outbox_send();
  } 
  
  Tuple *comp1_t = dict_find(iter, MESSAGE_KEY_Comp1);
  if(comp1_t) { 
    comp1 = atoi( comp1_t->value->cstring );
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Comp1: %d", comp1);
    
    persist_write_int(MESSAGE_KEY_Comp1, comp1);
  }
  
  Tuple *comp2_t = dict_find(iter, MESSAGE_KEY_Comp2);
  if(comp2_t) { 
    comp2 = atoi( comp2_t->value->cstring );
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Comp2: %d", comp2);
    
    persist_write_int(MESSAGE_KEY_Comp2, comp2);
  }
  
  Tuple *comp3_t = dict_find(iter, MESSAGE_KEY_Comp3);
  if(comp3_t) { 
    comp3 = atoi( comp3_t->value->cstring );
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Comp3: %d", comp3);
    
    persist_write_int(MESSAGE_KEY_Comp3, comp3);
  }
  
  Tuple *comp4_t = dict_find(iter, MESSAGE_KEY_Comp4);
  if(comp4_t) { 
    comp4 = atoi( comp4_t->value->cstring );
    
    //APP_LOG(APP_LOG_LEVEL_INFO, "Comp4: %d", comp4);
    
    persist_write_int(MESSAGE_KEY_Comp4, comp4);
  }

  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  
  if(comp1 == 5){text_layer_set_text(comp1_layer, weather_layer_buffer);}
  else if(comp2 == 5){text_layer_set_text(comp2_layer, weather_layer_buffer);}
  else if(comp3 == 5){text_layer_set_text(comp3_layer, weather_layer_buffer);}
  else if(comp4 == 5){text_layer_set_text(comp4_layer, weather_layer_buffer);} 
  
  update_time();
  battery_callback(battery_state_service_peek());
  handle_bluetooth(bluetooth_connection_service_peek());
  health_service_events_subscribe(handle_health, NULL);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	
	// Day
	comp1_layer = text_layer_create(GRect( PBL_IF_ROUND_ELSE(25, 0), PBL_IF_ROUND_ELSE(137, 137), 72, 25));
	text_layer_set_background_color(comp1_layer, GColorClear);
	text_layer_set_text_color(comp1_layer, GColorWhite);
	text_layer_set_text_alignment(comp1_layer, GTextAlignmentCenter);
	
	// Temperature
	comp2_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(84, 72), PBL_IF_ROUND_ELSE(137, 137), 72, 25));
	text_layer_set_background_color(comp2_layer, GColorClear);
	text_layer_set_text_color(comp2_layer, GColorWhite);
	text_layer_set_text_alignment(comp2_layer, GTextAlignmentCenter);
	
	// High Noon
	high_noon_layer = text_layer_create(GRect(0,  PBL_IF_ROUND_ELSE(98, 94), PBL_IF_ROUND_ELSE(178, 144), 50));
	text_layer_set_background_color(high_noon_layer, GColorClear);
	text_layer_set_text_color(high_noon_layer, GColorWhite);
	text_layer_set_text(high_noon_layer, "HIGH NOON");
	text_layer_set_text_alignment(high_noon_layer, GTextAlignmentCenter);
	layer_set_hidden(text_layer_get_layer(high_noon_layer), true);
	
	// Time
	s_time_layer = text_layer_create(GRect(0,  PBL_IF_ROUND_ELSE(98, 94), PBL_IF_ROUND_ELSE(178, 144), 50));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	GRect dummy_frame = { {0, 0}, {0, 0} };
	
	// Battery
	comp4_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(84, 72), PBL_IF_ROUND_ELSE(137, 137), 72, 25));
	text_layer_set_background_color(comp4_layer, GColorClear);
	text_layer_set_text_color(comp4_layer, GColorWhite);
	text_layer_set_text_alignment(comp4_layer, GTextAlignmentCenter);
	
	// Bluetooth
	comp3_layer = text_layer_create(GRect( PBL_IF_ROUND_ELSE(25, 0), PBL_IF_ROUND_ELSE(137, 137), 72, 25));
	text_layer_set_background_color(comp3_layer, GColorClear);
	text_layer_set_text_color(comp3_layer, GColorWhite);
	text_layer_set_text_alignment(comp3_layer, GTextAlignmentCenter);
  
	layer_set_hidden(text_layer_get_layer(comp3_layer), true);
	layer_set_hidden(text_layer_get_layer(comp4_layer), true);
	
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
	text_layer_set_font(comp1_layer, all_font);
	text_layer_set_font(comp2_layer, all_font);
	text_layer_set_font(comp4_layer, all_font);
	text_layer_set_font(comp3_layer, all_font);
	
	layer_add_child(window_layer, text_layer_get_layer(comp1_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(window_layer, text_layer_get_layer(high_noon_layer));
	layer_add_child(window_layer, text_layer_get_layer(comp2_layer));
	layer_add_child(window_layer, text_layer_get_layer(comp4_layer));
	layer_add_child(window_layer, text_layer_get_layer(comp3_layer));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
	text_layer_destroy(comp1_layer);
	text_layer_destroy(comp2_layer);
	text_layer_destroy(high_noon_layer);
	text_layer_destroy(comp4_layer);
	text_layer_destroy(comp3_layer);
	
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
  
  if(persist_exists(MESSAGE_KEY_Comp1)){
    comp1 = persist_read_int(MESSAGE_KEY_Comp1);}
  else{comp1 = 3;}
  
  if(persist_exists(MESSAGE_KEY_Comp2)){
    comp2 = persist_read_int(MESSAGE_KEY_Comp2);}
  else{comp2 = 5;}
  
  if(persist_exists(MESSAGE_KEY_Comp3)){
    comp3 = persist_read_int(MESSAGE_KEY_Comp3);}
  else{comp3 = 2;}
  
  if(persist_exists(MESSAGE_KEY_Comp4)){
    comp4 = persist_read_int(MESSAGE_KEY_Comp4);}
  else{comp4 = 1;}
  
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
  
	flick = persist_read_int(MESSAGE_KEY_Flick);
	hero_choice = persist_read_int(MESSAGE_KEY_Hero);
  
	if(hero_choice == 1000){
		update_display(tick_time);
	}
	else{
		set_container_image(&hero_images[0], hero_layers[0], hero[hero_choice], GPoint( PBL_IF_ROUND_ELSE(18, 0), PBL_IF_ROUND_ELSE(6, 0)));
	}
	
	update_bg(tick_time);
	initiate_watchface = false;
	
	// Register for battery level updates
	battery_state_service_subscribe(battery_callback);
	// Ensure battery level is displayed from the start
	battery_callback(battery_state_service_peek());
	
	handle_bluetooth(bluetooth_connection_service_peek());
	bluetooth_connection_service_subscribe(&handle_bluetooth);
  
  health_service_events_subscribe(handle_health, NULL);
	
	accel_tap_service_subscribe(&handle_tap);
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Open AppMessage connection
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(500,500);
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
	bluetooth_connection_service_unsubscribe();
	bluetooth_connection_service_peek();
}

int main(void) {
	init();
	app_event_loop();
	bluetooth_connection_service_subscribe(&handle_bluetooth);
	deinit();
}