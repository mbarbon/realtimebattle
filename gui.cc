#include <strstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>
#include "gui.h"
#include "Arena.h"
#include "MovingObject.h"
#include "Extras.h"
#include "Shape.h"
#include "Various.h"
#include "Options.h"
#include "Vector2D.h"

#define GDK_VARV 23040     // 64 * 360 degrees

static char * colour_square[] =
{
  "14 14 1 1",
  ".      c #000000000000 ",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  "..............",
  ".............."
};

void
no_zoom_callback(GtkWidget *widget, gpointer data)
{
  the_gui.change_zoom( NO_ZOOM );
}

void
zoom_in_callback(GtkWidget *widget, gpointer data)
{
  the_gui.change_zoom( ZOOM_IN );
}

void
zoom_out_callback(GtkWidget *widget, gpointer data)
{
  the_gui.change_zoom( ZOOM_OUT );
}

gint
redraw_arena (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  the_gui.draw_all_walls();
  return FALSE;
}

void
delete_event(GtkWidget *widget, gpointer data)
{
  the_gui.quit_event();
}

Gui::Gui()
{
  zoomfactor = 1;
  statistics_up = false;
  boundary[0] = Vector2D(0.0, 0.0);
  boundary[1] = Vector2D(0.0, 0.0);
  if(NULL != getenv("RTB_ROBOTDIR"))
      robotdir = getenv("RTB_ROBOTDIR");
  else
      robotdir = "Robots/";

  if(NULL != getenv("RTB_ARENADIR"))
      arenadir = getenv("RTB_ARENADIR");
  else
      arenadir = "Arenas/";
}

double
Gui::get_zoom()
{
  double w = (double)(drawing_area->allocation.width);
  double h = (double)(drawing_area->allocation.height);
  double bw = boundary[1][0] - boundary[0][0];
  double bh = boundary[1][1] - boundary[0][1];
  if( w / bw >= h / bh )
    return( h / bh );
  else
    return( w / bw );
}

void
Gui::change_zoom( const zoom_t type )
{
  switch( type )
    {
    case NO_ZOOM:
      zoomfactor = 1;
      break;
    case ZOOM_IN:
      zoomfactor++;
      break;
    case ZOOM_OUT:
      if(zoomfactor > 1)
        zoomfactor--;
      break;
    }
  gtk_widget_set_usize(drawing_area,
                       ( da_scrolled_window->allocation.width - 50 ) * zoomfactor,
                       ( da_scrolled_window->allocation.height - 50 ) * zoomfactor);
  draw_all_walls();
}

int
Gui::change_to_pixels_x(const double input)
{
  double res;
  res = (input-boundary[0][0])*get_zoom();
  return (int)res;
}

int
Gui::change_to_pixels_y(const double input)
{
  double res;
  res = (input-boundary[0][1])*get_zoom();
  return (int)res;
}

void
Gui::print_to_message_output (const String& from_robot, const String& output_text, GdkColor& colour )
{
  String rname = (String)'\n' + from_robot + ": ";
  String printed_text = output_text;
  gtk_text_freeze(GTK_TEXT(message_output));
  gtk_text_insert(GTK_TEXT(message_output), NULL, &colour, NULL, rname.chars(), -1);
  gtk_text_insert(GTK_TEXT(message_output), NULL, &message_output->style->black, NULL, printed_text.chars(), -1);
  gtk_text_thaw(GTK_TEXT(message_output));
}

char **
Gui::get_colour_square_xpm( char ** col_sq, const GdkColor& colour )
{
  strstream ss;
  char str[25];

  ss << ".      c #" << setfill('0') << hex << setw(4) << colour.red
     << setw(4) << colour.green << setw(4) << colour.blue << ends << endl;
  ss.getline(str,25,'\n');
  char* newstr = new char[30];
  strcpy(newstr, str);
  colour_square[1] = newstr;

  col_sq = (char **) colour_square;
  return col_sq;
}

void
Gui::draw_objects()
{
  GList** object_lists;
  GList* gl;
  Robot* robotp;

  object_lists = the_arena.get_object_lists();
  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)(gl->data);
      if( robotp->get_object_type() == ROBOT )
        if( robotp->is_alive() )
          {
            robotp->draw_shape( true );
            robotp->draw_radar_and_cannon();
          }
    }

  for(gl = g_list_next(object_lists[SHOT]); gl != NULL; gl = g_list_next(gl))
    if( ((Shot*)gl->data)->is_alive() )
      ((Shot*)gl->data)->draw_shape( true );

  for(gl = g_list_next(object_lists[MINE]); gl != NULL; gl = g_list_next(gl))
    if( ((Mine*)gl->data)->is_alive() )
      ((Mine*)gl->data)->draw_shape( true );

  for(gl = g_list_next(object_lists[COOKIE]); gl != NULL; gl = g_list_next(gl))
    if( ((Cookie*)gl->data)->is_alive() )
      ((Cookie*)gl->data)->draw_shape( true );
}

void
Gui::draw_all_walls()
{
  GList** object_lists;
  GList* gl;

  object_lists = the_arena.get_object_lists();
  for(gl = g_list_next(object_lists[WALL]); gl != NULL; gl = g_list_next(gl))
    ((Shape*)(WallCircle*)gl->data)->draw_shape( false ); // Strange, but it works!
}

void
Gui::draw_circle( const Vector2D& center, const double radius, GdkColor& colour, const bool filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  double x = max(1.0, 2.0*radius*get_zoom());
  double y = max(1.0, 2.0*radius*get_zoom());
  gdk_draw_arc (drawing_area->window,
                colour_gc,
                filled,
                change_to_pixels_x(center[0]-radius),change_to_pixels_y(center[1]-radius),
                (int)x, (int)y,
                0, GDK_VARV);

  gdk_gc_destroy( colour_gc );
}

void
Gui::draw_line(const Vector2D& start, const Vector2D& end, GdkColor& colour )
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
Gui::draw_line( const Vector2D& start, const Vector2D& direction, const double length, 
                const double thickness , GdkColor& colour )
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
  gdk_gc_set_foreground( colour_gc, the_arena.get_background_colour_p() );

  gdk_draw_rectangle (drawing_area->window,
                      colour_gc,
                      true,
                      0, 0, drawing_area->allocation.width,drawing_area->allocation.height);

  gdk_gc_destroy( colour_gc );
}

void
Gui::draw_rectangle( const Vector2D& start, const Vector2D& end, GdkColor& colour, const bool filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  gdk_draw_rectangle (drawing_area->window,
                      colour_gc,
                      filled,
                      change_to_pixels_x(start[0]), change_to_pixels_y(start[1]),
                      change_to_pixels_x(end[0] - start[0]), change_to_pixels_y(end[1] - start[1]));

  gdk_gc_destroy( colour_gc );
}
void
Gui::quit_event()
{
  gtk_main_quit();
}

void
Gui::setup_control_window()
{
  GtkWidget *button;
  GtkWidget *toptable,*bottomtable;
  GtkWidget *vbox;

  // Window 

  control_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (control_window), "RealTimeBattle Control");
  gtk_signal_connect (GTK_OBJECT (control_window), "delete_event",
                      GTK_SIGNAL_FUNC (delete_event), (gpointer) NULL);
  gtk_container_border_width (GTK_CONTAINER (control_window), 12);

  vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (control_window), vbox);
  gtk_widget_show (vbox);

  // Top Table 

  toptable = gtk_table_new (2, 10, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), toptable, FALSE, FALSE, 0);

  // Topmost buttons 

  button = gtk_button_new_with_label ("New Tournament");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (start_tournament_button_callback), (gpointer) NULL );
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
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (options_window_requested), (gpointer) NULL);
  gtk_table_attach_defaults (GTK_TABLE(toptable), button, 0, 5, 1, 2);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Statistics");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (statistics_button_callback), (gpointer) NULL );
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

  // Bottom Table 

  bottomtable = gtk_table_new (1, 4, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), bottomtable, FALSE, FALSE, 0);

  // Quit Button 

  button = gtk_button_new_with_label ("Quit");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (delete_event), (gpointer) NULL);
  gtk_table_attach_defaults (GTK_TABLE(bottomtable), button, 1, 3, 0, 1);
  gtk_widget_show (button);
  gtk_widget_show (bottomtable);

  gtk_widget_show (control_window);
}

void
Gui::set_score_window_title()
{
  String title = (String)"RealTimeBattle Score " + " Seq: " + the_arena.get_sequence_nr() + " Game: " +
    String(the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence()) +
    " Time: " + String((int)the_arena.get_total_time());
  gtk_window_set_title (GTK_WINDOW (score_window), title.chars());
}

void
Gui::setup_score_window()
{
  int robot_number=0;

  GList** object_lists;
  GList* gl;
  Robot* robotp;

  object_lists = the_arena.get_object_lists();
  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    robot_number++;

  // Window 

  char * titles[6] = { "", "Name", "Energy ", "Place ", "Last ", "Score  " };

  score_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  set_score_window_title();
  gtk_signal_connect (GTK_OBJECT (score_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(score_window));
  gtk_container_border_width (GTK_CONTAINER (score_window), 12);

  score_clist = gtk_clist_new_with_titles( 6, titles);
  gtk_clist_set_selection_mode (GTK_CLIST(score_clist), GTK_SELECTION_EXTENDED);
  gtk_clist_set_border(GTK_CLIST(score_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 0, 20);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 1, 80);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 2, 44);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 3, 38);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 4, 35);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 5, 50);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 0, GTK_JUSTIFY_CENTER);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 2, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 3, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 4, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 5, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_policy(GTK_CLIST(score_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(score_clist, 335, 350);
  gtk_container_add (GTK_CONTAINER (score_window), score_clist);
  gtk_widget_show(score_clist);

  gtk_widget_show (score_window);

  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)(gl->data);

      char * list[6];

      for(int j=0;j<6;j++)
        {
          list[j] = new char[30];
          strcpy(list[j],"");
        }

      int row = gtk_clist_append(GTK_CLIST(score_clist), list);
      gtk_clist_set_foreground(GTK_CLIST(score_clist), row, the_arena.get_foreground_colour_p());
      gtk_clist_set_background(GTK_CLIST(score_clist), row, the_arena.get_background_colour_p());

      robotp->set_row_in_score_clist( row );

      GdkPixmap * colour_pixmap;
      GdkBitmap * bitmap_mask;
      char ** col_sq;

      colour_pixmap = gdk_pixmap_create_from_xpm_d( score_window->window, &bitmap_mask,
                                                    the_arena.get_background_colour_p(),
                                                    get_colour_square_xpm( col_sq, robotp->get_colour() ) );

      gtk_clist_set_pixmap(GTK_CLIST(score_clist), row, 0, colour_pixmap, bitmap_mask);

      gtk_clist_set_text(GTK_CLIST(score_clist), row, 1, robotp->get_robot_name().non_const_chars());

      gtk_clist_set_text(GTK_CLIST(score_clist), row, 3, "");
      robotp->reset_last_displayed();
      robotp->display_score();
    }
}

void
Gui::setup_message_window()
{
  GtkWidget * vbox;
  GtkWidget * messagetable;
  GtkWidget * vscrollbar;

  // Window 

  message_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (message_window), "RealTimeBattle Messages");
  gtk_signal_connect (GTK_OBJECT (message_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(message_window));
  gtk_container_border_width (GTK_CONTAINER (message_window), 12);

  // The VBox 

  vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (message_window), vbox);
  gtk_widget_show (vbox);

  // Message Table 

  messagetable = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (messagetable), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (messagetable), 0, 2);
  gtk_box_pack_start (GTK_BOX (vbox), messagetable, TRUE, TRUE, 0);
  gtk_widget_show (messagetable);

  // Message Text 

  message_output = gtk_text_new (NULL, NULL);
  gtk_text_set_editable (GTK_TEXT (message_output), FALSE);
  gtk_table_attach (GTK_TABLE (messagetable), message_output, 0, 1, 0, 1,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
                    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_set_usize(message_output, 250,90);
  gtk_widget_show(message_output);

  // Message VScrollBar 

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (message_output)->vadj);
  gtk_table_attach (GTK_TABLE (messagetable), vscrollbar, 1, 2, 0, 1,
                    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);

  gtk_widget_realize (message_output);
  gtk_text_freeze (GTK_TEXT (message_output));

  gtk_widget_show (message_window);
}

void
Gui::setup_arena_window( const Vector2D bound[] )
{
  GtkWidget * vbox, * button_table;
  GtkWidget * button;

  boundary[0] = bound[0];
  boundary[1] = bound[1];

  // Window 

  arena_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (arena_window), "RealTimeBattle Arena");
  gtk_signal_connect (GTK_OBJECT (arena_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(arena_window));
  gtk_container_border_width (GTK_CONTAINER (arena_window), 12);  

  // VBox

  vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (arena_window), vbox);
  gtk_widget_show (vbox);

  // Zoom Buttons etc.

  button_table = gtk_table_new (1, 3, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), button_table, FALSE, FALSE, 0);

  button = gtk_button_new_with_label ("No Zoom");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (no_zoom_callback), (gpointer) NULL);
  gtk_table_attach_defaults (GTK_TABLE(button_table), button, 0, 1, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Zoom In");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (zoom_in_callback), (gpointer) NULL);
  gtk_table_attach_defaults (GTK_TABLE(button_table), button, 1, 2, 0, 1);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Zoom Out");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (zoom_out_callback), (gpointer) NULL);
  gtk_table_attach_defaults (GTK_TABLE(button_table), button, 2, 3, 0, 1);
  gtk_widget_show (button);

  gtk_table_set_col_spacings (GTK_TABLE(button_table), 5);
  gtk_widget_show (button_table);

  // Scrolled Window 

  da_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (da_scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (vbox), da_scrolled_window);
  gtk_widget_set_usize(da_scrolled_window,404,404);
  gtk_widget_show (da_scrolled_window);

  // Drawing Area 

  drawing_area = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area),400,400);
  gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
                      (GtkSignalFunc) redraw_arena, (gpointer) NULL);
  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK);
  gtk_container_add (GTK_CONTAINER (da_scrolled_window),drawing_area);
  gtk_widget_show (drawing_area);

  gtk_widget_show (arena_window);  

  // Background Colour 

  gdk_window_set_background (drawing_area->window, the_arena.get_background_colour_p());
  gdk_window_clear (drawing_area->window);

  clear_area();
}

void
Gui::close_control_window()
{
  gtk_widget_destroy ( control_window );
}

void
Gui::close_score_window()
{
  gtk_widget_destroy ( score_window );
}

void
Gui::close_message_window()
{
  gtk_widget_destroy ( message_window );
}

void
Gui::close_arena_window()
{
  gtk_widget_destroy ( arena_window );
}
