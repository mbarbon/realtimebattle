#include <math.h>
#include "gui.h"
#include "Vector2D.h"
#include "Arena.h"

#define GDK_VARV 23040

void
delete_event (GtkWidget *widget, GdkEvent *event, gpointer guip)
{
  ((Gui *)guip)->quit_event( widget, event );
}

Gui::Gui()
{
  boundary[0] = Vector2D(0.0, 0.0);
  boundary[1] = Vector2D(0.0, 0.0);
}

int
Gui::change_to_pixels_x(double input)
{
  double res;
  res = (input-boundary[0][0])*ZOOMFACTOR;
  return (int)res;
}

int
Gui::change_to_pixels_y(double input)
{
  double res;
  res = (input-boundary[0][1])*ZOOMFACTOR;
  return (int)res;
}

void
Gui::print_to_message_output (char * from_robot, char * output_text, GdkColor colour )
{
  GString * rname;
  rname = g_string_new(from_robot);
  g_string_prepend(rname,"\n");
  g_string_append(rname,": ");
  gtk_text_freeze(GTK_TEXT(message_output));
  gtk_text_insert(GTK_TEXT(message_output), NULL, &colour, NULL, rname->str, -1);
  gtk_text_insert(GTK_TEXT(message_output), NULL, &message_output->style->black, NULL, output_text, -1);
  gtk_text_thaw(GTK_TEXT(message_output));
}

void
Gui::draw_objects( gpointer the_arenap )
{
  GList** object_lists;
  GList* gl;
  Robot* robotp;

  clear_area();
  object_lists = ((Arena *)the_arenap)->get_object_lists();
  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)(gl->data);
      if( robotp->get_object_type() == ROBOT )
        if( robotp->is_alive() )
            robotp->draw_shape( *this );
    }
  for(gl = g_list_next(object_lists[WALL]); gl != NULL; gl = g_list_next(gl))
    {
      ((Shape*)(WallCircle*)gl->data)->draw_shape( *this ); // Strange, but it works!
    }      
}

void
Gui::draw_circle( Vector2D center, double radius, GdkColor colour, bool filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  gdk_draw_arc (drawing_area->window,
                colour_gc,
                filled,
                change_to_pixels_x(center[0]-radius),change_to_pixels_y(center[1]-radius),
                (int)(2.0*radius*ZOOMFACTOR), (int)(2.0*radius*ZOOMFACTOR),
                0, GDK_VARV);

  gdk_gc_destroy( colour_gc );
}

void
Gui::draw_line( Vector2D start, Vector2D end, GdkColor colour )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  gdk_draw_line (drawing_area->window,
                 colour_gc,
                 change_to_pixels_x(start[0]), change_to_pixels_y(start[1]),
                 change_to_pixels_x(end[0]), change_to_pixels_y(start[1]));

  gdk_gc_destroy( colour_gc );
}

void
Gui::draw_line( Vector2D start, Vector2D direction, double length, double thickness , GdkColor colour )
{
  GdkGC * colour_gc;
  GdkPoint g_points[4];
  Vector2D vector_points[4];

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  Vector2D line_thick = unit(direction);
  line_thick = rotate90(line_thick);
  line_thick *= thickness;
  vector_points[0] = start + line_thick;
  vector_points[1] = start - line_thick;
  vector_points[2] = start - line_thick + direction * length;
  vector_points[3] = start + line_thick + direction * length;

  for(int i=0;i<4;i++)
    {
      g_points[i].x = change_to_pixels_x(vector_points[i][0]);
      g_points[i].y = change_to_pixels_y(vector_points[i][1]);
    }
  gdk_draw_polygon (drawing_area->window, colour_gc, true, g_points, 4);

  gdk_gc_destroy( colour_gc );
}

void
Gui::clear_area()
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &background_colour );

  gdk_draw_rectangle (drawing_area->window,
                      colour_gc,
                      true,
                      0, 0, drawing_area->allocation.width,drawing_area->allocation.height);

  gdk_gc_destroy( colour_gc );
}

void
Gui::draw_rectangle( Vector2D start, Vector2D end, GdkColor colour, bool filled )
{
  GdkGC * colour_gc;

  gdk_color_alloc (colormap, &background_colour);
  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  gdk_draw_rectangle (drawing_area->window,
                      colour_gc,
                      filled,
                      change_to_pixels_x(start[0]), change_to_pixels_y(start[1]),
                      change_to_pixels_x(end[0]), change_to_pixels_y(start[1]));

  gdk_gc_destroy( colour_gc );
}

void
Gui::quit_event (GtkWidget *widget, GdkEvent *event)
{
  gtk_main_quit ();
}

void
Gui::setup_control_window( char * robot_name_list[] )
{
  int robot_number=0;

  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *toptable,*messagetable,*bottomtable,*rltable,*rhtable;
  GtkWidget *life,*score,*place;
  GtkWidget *label;
  GtkWidget *vbox,*scr_vbox,*hbox, *robothbox;
  GtkWidget *scrolled_window;
  GtkWidget * vscrollbar;
  GdkColormap *cmap;

  for( int i=0; robot_name_list[i] != NULL;i++)
    robot_number++;

  /* Window */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "RealTimeBattle Control Window");
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (delete_event), this);
  gtk_container_border_width (GTK_CONTAINER (window), 12);

  vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  /* Top Table */

  toptable = gtk_table_new (2, 10, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), toptable, FALSE, FALSE, 0);

  /* Topmost buttons */

  button = gtk_button_new_with_label ("New Tournament");
  //  gtk_signal_connect (GTK_OBJECT (button), "clicked",
  //                      GTK_SIGNAL_FUNC (callback), (gpointer) "Tournament");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 0, 4, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("New Game");
  //  gtk_signal_connect (GTK_OBJECT (button), "clicked",
  //                      GTK_SIGNAL_FUNC (callback), (gpointer) "Game");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 4, 7, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("End");
  //  gtk_signal_connect (GTK_OBJECT (button), "clicked",
  //                      GTK_SIGNAL_FUNC (callback), (gpointer) "End");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 7, 10, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Options");
  //  gtk_signal_connect (GTK_OBJECT (button), "clicked",
  //                      GTK_SIGNAL_FUNC (callback), (gpointer) "Options");
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 0, 5, 1, 2);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Statistics");
  //  gtk_signal_connect (GTK_OBJECT (button), "clicked",
  //                      GTK_SIGNAL_FUNC (callback), (gpointer) "Statistics");
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

  rltable = gtk_table_new (robot_number, 1, TRUE);
  gtk_box_pack_start (GTK_BOX (robothbox), rltable, FALSE, FALSE, 0);

  rhtable = gtk_table_new (robot_number, 1, TRUE);
  gtk_box_pack_start (GTK_BOX (robothbox), rhtable, FALSE, FALSE, 0);

  /* Robots */

  for(int i=0;robot_name_list[i] != NULL ;i++) {

    hbox = gtk_hbox_new (FALSE, 10);
    gtk_table_attach_defaults (GTK_TABLE(rhtable), hbox, 0, 1, i, i + 1);
    gtk_widget_show (hbox);  

    /* Robot Buttons */

    label = gtk_label_new (robot_name_list[i]);
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

  message_output = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT (message_output), FALSE);
  gtk_table_attach (GTK_TABLE (messagetable), message_output, 0, 1, 0, 1,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_set_usize(message_output, 160,63);
  gtk_widget_show(message_output);

  /* Message VScrollBar */

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (message_output)->vadj);
  gtk_table_attach (GTK_TABLE (messagetable), vscrollbar, 1, 2, 0, 1,
                    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);

  cmap = gdk_colormap_get_system();
  blue_colour.red = 0;
  blue_colour.green = 0;
  blue_colour.blue = 0xaaaa;

  gtk_widget_realize (message_output);
  gtk_text_freeze (GTK_TEXT (message_output));

  /* Bottom Table */

  bottomtable = gtk_table_new (1, 4, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), bottomtable, FALSE, FALSE, 0);

  /* Quit Button */

  button = gtk_button_new_with_label ("Quit");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (delete_event), this);
  gtk_table_attach_defaults (GTK_TABLE(bottomtable), button, 1, 3, 0, 1);
  gtk_widget_show (button);
  gtk_widget_show (bottomtable);

  gtk_widget_show (window);
}

void
Gui::setup_arena_window( Vector2D bound[] )
{
  GtkWidget *window;

  boundary[0] = bound[0];
  boundary[1] = bound[1];

  /* Window */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "RealTimeBattle Arena Window");
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (delete_event), this);
  gtk_container_border_width (GTK_CONTAINER (window), 12);  

  /* Scrolled Window */

  da_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (da_scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (window), da_scrolled_window);
  gtk_widget_set_usize(da_scrolled_window,200,200);
  gtk_widget_show (da_scrolled_window);

  /* Drawing Area */

  drawing_area = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area),
                         change_to_pixels_x(boundary[1][0]),
                         change_to_pixels_y(boundary[1][1]));
  gtk_container_add (GTK_CONTAINER (da_scrolled_window),drawing_area);
  gtk_widget_show (drawing_area);

  gtk_widget_show (window);  

  /* Background Colour */

  colormap = gdk_window_get_colormap (drawing_area->window);
  background_colour.red = 64255;
  background_colour.green = 61695;
  background_colour.blue = 59135;
  gdk_color_alloc (colormap, &background_colour);
  gdk_window_set_background (drawing_area->window, &background_colour);
  gdk_window_clear (drawing_area->window);
}
