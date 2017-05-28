#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static void main_window_load(Window *window) {
    // Obtenemos información acerca de la ventana
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Creamos el TextLayer con las dimensiones específicas
    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
    
    // Mejoramos la alineación para que se parezca más a la de un watchface
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    // Se añade como capa hija a la capa raíz de la Window
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    // Destruimos TextLayer
    text_layer_destroy(s_time_layer);
}

static void update_time(struct tm *tick_time) {

    // Escribimos la hora minutos actuales en un buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    // Mostramos esta hora en el TextLayer
    text_layer_set_text(s_time_layer, s_buffer);
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