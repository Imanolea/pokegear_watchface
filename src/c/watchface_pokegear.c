#include <pebble.h>

static Window *s_main_window;

static void main_window_load(Window *window) {

}

static void main_window_unload(Window *window) {

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