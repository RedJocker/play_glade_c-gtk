// clang $(pkg-config --cflags gtk+-3.0) -g -rdynamic ./glade_play.c -o ./glade_play $(pkg-config --libs gtk+-3.0) && ./glade_play

#include <gtk/gtk.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "./observing.c"

typedef struct {
  GtkWidget *text_label;
  GtkWidget *window;
  GtkWidget *button_a;
  GtkWidget *button_b;
  GtkWidget *button_c;
  Subject *counter;
} Screen;

static GtkWidget *text_label;

void button_a_clicked_cb (GtkButton *b, gpointer user_data) {
  Subject *counter = (Subject*) user_data;
  set_state(counter,  counter->state + 1);
  printf("HELLO_A %d subject %p\n", counter->state, counter);
}

void button_b_clicked_cb (GtkButton *b, gpointer user_data) {
  Subject *counter = (Subject*) user_data;
  set_state(counter,  0);
  printf("HELLO_B %d subject %p\n", counter->state, counter);
}

void button_c_clicked_cb (GtkButton *b, gpointer user_data) {
  Subject *counter = (Subject*) user_data;
  set_state(counter,  counter->state - 1);
  printf("HELLO_C %d subject %p\n", counter->state, counter);
}

void on_counter_change (int state) {
  printf("on_counter_change %d/n", state);
  char* str;
  sprintf(str, "%d", state);
  gtk_label_set_text (GTK_LABEL(text_label), (const gchar*) str);
}

void build_screen(Screen *screen) {

  GtkBuilder *builder = gtk_builder_new_from_file("./gladePlay.glade");
  screen->window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  g_signal_connect(screen->window,
		   "destroy",
		   G_CALLBACK(gtk_main_quit),
		   NULL);

  screen->button_a =
    GTK_WIDGET(gtk_builder_get_object(builder, "button_a"));

  screen->button_b =
    GTK_WIDGET(gtk_builder_get_object(builder, "button_b"));

  screen->button_c =
    GTK_WIDGET(gtk_builder_get_object(builder, "button_c"));

  screen->text_label =
    GTK_WIDGET(gtk_builder_get_object(builder, "text_label"));
  text_label = screen->text_label;
}

void set_screen_listeners(Screen *screen) {
  g_signal_connect(screen->button_a,
		   "clicked",
		   G_CALLBACK(button_a_clicked_cb),
		   (gpointer) screen->counter);
  g_signal_connect(screen->button_b,
		   "clicked",
		   G_CALLBACK(button_b_clicked_cb),
		   (gpointer) screen->counter);
  g_signal_connect(screen->button_c,
		   "clicked",
		   G_CALLBACK(button_c_clicked_cb),
		   (gpointer) screen->counter);
}

void set_observers(Screen *screen) {

  Observer *observer = malloc(sizeof(Observer));
  observer->update = &print_on_update;

  Observer *logger = malloc(sizeof(Observer));
  logger->update = &log_on_update;
  
  Observer *label_updater = malloc(sizeof(Observer));
  label_updater->update = &on_counter_change;
 
  screen->counter->observers = malloc(2 * sizeof(Observer*));
  screen->counter->observer_count = 0;
  screen->counter->state = 0;

  add_observer(screen->counter, observer);
  add_observer(screen->counter, logger);
  add_observer(screen->counter, label_updater);
  printf("after connect: observer count %d %p\n",
	 screen->counter->observer_count,
	 screen->counter);
}

void start(Screen *screen) {
  
  build_screen(screen);
  set_screen_listeners(screen);
  set_observers(screen);
  
  gtk_widget_show(screen->window);
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    Screen *screen = malloc(sizeof(Screen));
    screen->counter = malloc(sizeof(Subject*));

    start(screen);
    
    gtk_main();

    for (size_t i = 0; i < screen->counter->observer_count; i++) {
      free(screen->counter->observers[i]);
    }
    
    free(screen->counter->observers);
    free(screen->counter);
    free(screen);
    return 0;
}
