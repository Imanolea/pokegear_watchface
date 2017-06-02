#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_am_layer;
static TextLayer *s_pm_layer;
static GFont s_time_font;
static GFont s_ampm_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void main_window_load(Window *window) {
    // Obtenemos información acerca de la ventana
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Creamos un GBitmap del fondo
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

    // Creamos un BitmapLayer para mostrar el GBitmap
    s_background_layer = bitmap_layer_create(bounds);

    // Seteamos el bitmap en la capa y los añadimos a la ventana
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

    // Creamos el TextLayer con las dimensiones específicas
    s_time_layer = text_layer_create(
        GRect(67, 3, bounds.size.w, 50));
    // Creamos los TextLayer de las letras de am/pm
    s_am_layer = text_layer_create(GRect(48, 3, 14, 14));
    s_pm_layer = text_layer_create(GRect(48, 15, 14, 14));
    
    // Mejoramos la alineación para que se parezca más a la de un watchface
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text(s_time_layer, "00:00");
    // Configuramos la capa de las letras de am/pm
    text_layer_set_background_color(s_am_layer, GColorClear);
    text_layer_set_text_color(s_am_layer, GColorBlack);
    text_layer_set_background_color(s_pm_layer, GColorClear);
    text_layer_set_text_color(s_pm_layer, GColorBlack);

    // Creación de la fuente
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_25));
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
    s_ampm_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_12));
    text_layer_set_font(s_am_layer, s_ampm_font);
    text_layer_set_text_alignment(s_am_layer, GTextAlignmentLeft);
    text_layer_set_font(s_pm_layer, s_ampm_font);
    text_layer_set_text_alignment(s_pm_layer, GTextAlignmentLeft);

    // Se añade como capa hija a la capa raíz de la Window
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_am_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_pm_layer));
}

static void main_window_unload(Window *window) {
    // Destruimos TextLayer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_am_layer);
    text_layer_destroy(s_pm_layer);
    // Descargamos la GFont
    fonts_unload_custom_font(s_time_font);
    fonts_unload_custom_font(s_ampm_font);
    // Destruimos el GBitmap
    gbitmap_destroy(s_background_bitmap);
    // Destruimos el BitmapLayer
    bitmap_layer_destroy(s_background_layer);
}

static void update_time(struct tm *tick_time) {
    bool clock_is_24h = clock_is_24h_style();

    // Escribimos la hora minutos actuales en un buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h ? "%H:%M":"%I:%M", tick_time);

    static char s_ampm[8];
    strftime(s_ampm, sizeof(s_ampm), clock_is_24h ? "": "%p", tick_time);


    // Mostramos esta hora en el TextLayer
    text_layer_set_text(s_time_layer, s_buffer);
    if (s_ampm[0] == 'a') {
        text_layer_set_text(s_am_layer, s_ampm);
    } else {
        text_layer_set_text(s_pm_layer, s_ampm);
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(tick_time);
}

static void init() {
    // Creamos el elemento principal de Window y lo asignamos al puntero
    s_main_window = window_create();

    // Seteamos los handlers para controlar los elementos de dentro de la ventana
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    // Muestra la Window en el reloj, con animated=true
    window_stack_push(s_main_window, true);

    // Nos registramos con TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    // Nos aseguramos de que la hora se muetra desde el principio
    time_t temp = time(NULL);
    update_time(localtime(&temp));
    // Fondo de color blanco
    window_set_background_color(s_main_window, GColorWhite);
}

static void deinit() {
    // Destruye la ventana
    window_destroy(s_main_window);   
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}