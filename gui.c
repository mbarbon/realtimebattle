#include <math.h>
#include "gui.h"

GtkWidget * message;
GtkWidget *vscrollbar;
GdkColor colour;

void insert_message (char * robot_name, char * write_message)
{
  GString * rname;
  rname = g_string_new(robot_name);
  g_string_prepend(rname,"\n");
  g_string_append(rname,": ");
  gtk_text_freeze(GTK_TEXT(message));
  gtk_text_insert(GTK_TEXT(message), NULL, &colour, NULL, rname->str, -1);
  gtk_text_insert(GTK_TEXT(message), NULL, &message->style->black, NULL, write_message, -1);
  gtk_text_thaw(GTK_TEXT(message));
}

void callback (GtkWidget *widget, gpointer data)
{
}

void delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
  gtk_main_quit ();
}


void control_win( int antrobotar, char * robotnames[] )
{
  int i;

  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *toptable,*messagetable,*bottomtable,*rltable,*rhtable;
  GtkWidget *life,*score,*place;
  GtkWidget *label;
  GtkWidget *vbox,*scr_vbox,*hbox, *robothbox;
  GtkWidget *scrolled_window;
  GdkColormap *cmap;

  /* Window */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "RobotArena");
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (delete_event), NULL);
  gtk_container_border_width (GTK_CONTAINER (window), 12);

  vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  /* Top Table */

  toptable = gtk_table_new (2, 10, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), toptable, FALSE, FALSE, 0);

  /* Topmost buttons */

  button = gtk_button_new_with_label ("New Tournament");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (callback), (gpointer) "Tournament");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 0, 4, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("New Game");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (callback), (gpointer) "Game");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 4, 7, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("End");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (callback), (gpointer) "End");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 7, 10, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Options");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (callback), (gpointer) "Options");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 0, 5, 1, 2);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Statistics");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (callback), (gpointer) "Statistics");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 5, 10, 1, 2);
  gtk_widget_show (button);

  gtk_table_set_row_spacing (GTK_TABLE(toptable), 0, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 0, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 1, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 2, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 3, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 4, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 5, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 6, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 7, 10);
  gtk_table_set_col_spacing (GTK_TABLE(toptable), 8, 10);
  gtk_widget_show (toptable);

  /* Scrolled Window */

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (vbox), scrolled_window);
  gtk_widget_set_usize(scrolled_window,260,62);
  // Y=ANTAL_ROBOTAR*30 + 14
  gtk_widget_show (scrolled_window);

  /* VBox in Scrolled Window */

  scr_vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_border_width (GTK_CONTAINER (scr_vbox), 10);
  gtk_container_add (GTK_CONTAINER (scrolled_window), scr_vbox);
  gtk_widget_show (scr_vbox);

  /* Robot Tables */

  robothbox = gtk_hbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (scr_vbox), robothbox);
  gtk_widget_show (robothbox);  

  rltable = gtk_table_new (antrobotar, 1, TRUE);
  gtk_box_pack_start (GTK_BOX (robothbox), rltable, FALSE, FALSE, 0);

  rhtable = gtk_table_new (antrobotar, 1, TRUE);
  gtk_box_pack_start (GTK_BOX (robothbox), rhtable, FALSE, FALSE, 0);

  /* Robots */

  for(i=0;i<antrobotar;i++) {

    hbox = gtk_hbox_new (FALSE, 10);
    gtk_table_attach_defaults (GTK_TABLE(rhtable), hbox, 0, 1, i, i + 1);
    gtk_widget_show (hbox);  

    /* Robot Buttons */

    label = gtk_label_new (robotnames[i]);
    gtk_table_attach_defaults (GTK_TABLE(rltable), label, 0, 1, i, i + 1);
    gtk_widget_show (label);

    label = gtk_label_new ("Life");
    gtk_container_add (GTK_CONTAINER (hbox), label);
    gtk_widget_show (label);

    life = gtk_entry_new_with_max_length(3);
    gtk_entry_set_text (GTK_ENTRY (life), "");
    gtk_entry_set_editable(GTK_ENTRY(life),FALSE);
    gtk_widget_set_usize(life, 30,25);
    gtk_container_add (GTK_CONTAINER (hbox), life);
    gtk_widget_show (life);

    label = gtk_label_new ("Place");
    gtk_container_add (GTK_CONTAINER (hbox), label);
    gtk_widget_show (label);

    place = gtk_entry_new_with_max_length(2);
    gtk_entry_set_text (GTK_ENTRY (place), "");
    gtk_entry_set_editable(GTK_ENTRY(place),FALSE);
    gtk_widget_set_usize(place, 24,25);
    gtk_container_add (GTK_CONTAINER (hbox), place);
    gtk_widget_show (place);

    label = gtk_label_new ("Score");
    gtk_container_add (GTK_CONTAINER (hbox), label);
    gtk_widget_show (label);

    score = gtk_entry_new_with_max_length(4);
    gtk_entry_set_text (GTK_ENTRY (score), "");
    gtk_entry_set_editable(GTK_ENTRY(score),FALSE);
    gtk_widget_set_usize(score, 36,25);
    gtk_container_add (GTK_CONTAINER (hbox), score);
    gtk_widget_show (score);
  }

  gtk_table_set_row_spacings (GTK_TABLE(rhtable), 5);
  gtk_table_set_row_spacings (GTK_TABLE(rltable), 5);
  gtk_widget_show (rhtable);
  gtk_widget_show (rltable);

  /* Message Table */

  messagetable = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (messagetable), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (messagetable), 0, 2);
  gtk_box_pack_start (GTK_BOX (vbox), messagetable, FALSE, FALSE, 0);
  gtk_widget_show (messagetable);

  /* Message Text */

  message = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT (message), FALSE);
  gtk_table_attach (GTK_TABLE (messagetable), message, 0, 1, 0, 1,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_set_usize(message, 160,63);
  gtk_widget_show(message);

  /* Message VScrollBar */

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (message)->vadj);
  gtk_table_attach (GTK_TABLE (messagetable), vscrollbar, 1, 2, 0, 1,
                    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);

  cmap = gdk_colormap_get_system();
  colour.red = 0;
  colour.green = 0;
  colour.blue = 0xaaaa;
  if (!gdk_color_alloc(cmap, &colour)) {
    g_error("couldn't allocate colour");
  }

  gtk_widget_realize (message);
  gtk_text_freeze (GTK_TEXT (message));

  //  gtk_text_insert(GTK_TEXT(message), NULL, &colour, NULL, "\nWalkfire: ", -1);
  //  gtk_text_insert(GTK_TEXT(message), NULL, &message->style->black, NULL, "Detta meddelande är ett meddelande vars text är mycket lång", -1);
/*   gtk_text_insert(GTK_TEXT(message), NULL, &colour, NULL, "\nWalkfire: ", -1); */
/*   gtk_text_insert(GTK_TEXT(message), NULL, &message->style->black, NULL, "Detta meddelande är ett meddelande vars text är mycket lång", -1); */

  gtk_text_set_point (GTK_TEXT(message), gtk_text_get_length (GTK_TEXT(message)));

  /* Bottom Table */

  bottomtable = gtk_table_new (1, 4, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), bottomtable, FALSE, FALSE, 0);

  /* Quit Button */

  button = gtk_button_new_with_label ("Quit");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (delete_event), NULL);
  gtk_table_attach_defaults (GTK_TABLE(bottomtable), button, 1, 3, 0, 1);
  gtk_widget_show (button);
  gtk_widget_show (bottomtable);

  gtk_widget_show (window);
}

void arena_win( void )
{
  GtkWidget *window;
  GtkWidget *scrolled_window;
  GtkWidget *drawing_area;

  GdkColor gdk_color;
  GdkColormap *colormap;

  /* Window */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "RobotArena");
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (delete_event), NULL);
  gtk_container_border_width (GTK_CONTAINER (window), 12);  

  /* Scrolled Window */

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (window), scrolled_window);
  gtk_widget_set_usize(scrolled_window,100,100);
  gtk_widget_show (scrolled_window);

  /* Drawing Area */

  drawing_area = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 1000, 1000);
  gtk_container_add (GTK_CONTAINER (scrolled_window),drawing_area);
  gtk_widget_show (drawing_area);

  gtk_widget_show (window);  

  /* Background Colour */

  colormap = gdk_window_get_colormap (drawing_area->window);
  gdk_color.red = 64255;
  gdk_color.green = 61695;
  gdk_color.blue = 59135;
  gdk_color_alloc (colormap, &gdk_color);
  gdk_window_set_background (drawing_area->window, &gdk_color);
  gdk_window_clear (drawing_area->window);

}

