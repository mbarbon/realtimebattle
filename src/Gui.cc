/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <strstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <math.h>
//#include <string.h>
#include <stdlib.h>
#include "Gui.h"
#include "Arena.h"
#include "Shot.h"
#include "Robot.h"
#include "Extras.h"
#include "Various.h"
#include "Options.h"
#include "Vector2D.h"
#include "Wall.h"

#define GDK_360_DEGREES 23040     // 64 * 360 degrees

#define call_function(function_name) function_name()

void
question_yes_callback(GtkWidget * widget, QuestionFunction function_name)
{
  (*function_name)(true);
  the_gui.close_question_window();
}

void
question_no_callback(GtkWidget * widget, QuestionFunction function_name)
{
  (*function_name)(false);
  the_gui.close_question_window();
}

void
new_robot_selected( GtkWidget * clist, gint row, gint column,
                    GdkEventButton *event, gpointer data )
{
  if( event != NULL )
    the_gui.change_selected_robot( row );
}

void
no_zoom_callback(GtkWidget *widget, gpointer data)
{
  the_gui.change_zoomfactor( NO_ZOOM );
}

void
zoom_in_callback(GtkWidget *widget, gpointer data)
{
  the_gui.change_zoomfactor( ZOOM_IN );
}

void
zoom_out_callback(GtkWidget *widget, gpointer data)
{
  the_gui.change_zoomfactor( ZOOM_OUT );
}

gint
redraw_arena (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  if(the_arena.get_state() == GAME_IN_PROGRESS)
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
  statistics_up = false;

  messagewindow_p = NULL;

  da_scrolled_window_size = Vector2D(0.0,0.0);
}

void
Gui::change_zoom()
{
  int width = da_scrolled_window->allocation.width - 24;
  int height = da_scrolled_window->allocation.height - 24;
  da_scrolled_window_size = Vector2D((double)width,(double)height);
  double w = (double)(width * zoomfactor);
  double h = (double)(height * zoomfactor);
  double bw = the_arena.get_boundary()[1][0] - the_arena.get_boundary()[0][0];
  double bh = the_arena.get_boundary()[1][1] - the_arena.get_boundary()[0][1];
  if( w / bw >= h / bh )
    {
      zoom = h / bh;
      w = zoom*bw;
    }
  else
    {
      zoom = w / bw;
      h = zoom*bh;
    }

  gtk_widget_set_usize(drawing_area,(int)w,int(h));
  clear_area();
  draw_all_walls();
}

void
Gui::change_zoomfactor( const zoom_t type )
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
  change_zoom();
}

int
Gui::change_to_pixels_x(const double input)
{
  double res;
  res = (input-the_arena.get_boundary()[0][0])*zoom + 0.5;
  return (int)res;
}

int
Gui::change_to_pixels_y(const double input)
{
  double res;
  res = (input-the_arena.get_boundary()[0][1])*zoom + 0.5;
  return (int)res;
}

void
Gui::draw_objects(const bool clear_objects_first)
{
  GList** object_lists;
  GList* gl;
  Robot* robotp;

  if((da_scrolled_window->allocation.width - 24 !=  da_scrolled_window_size[0]) ||
     (da_scrolled_window->allocation.height - 24 !=  da_scrolled_window_size[1]))
    change_zoom();

  object_lists = the_arena.get_object_lists();
  for(gl = g_list_next(object_lists[SHOT_T]); gl != NULL; gl = g_list_next(gl))
    if( ((Shot*)gl->data)->is_alive() )
      ((Shot*)gl->data)->draw_shape( clear_objects_first );

  for(gl = g_list_next(object_lists[ROBOT_T]); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)(gl->data);

      //      if( robotp->get_arenaobject_t() == ROBOT ) // is this needed??
      if( robotp->is_alive() )
        {
          robotp->draw_shape( clear_objects_first );
          robotp->draw_radar_and_cannon();
        }
    }
}

void
Gui::draw_all_walls()
{
  GList** object_lists;
  GList* gl;

  object_lists = the_arena.get_object_lists();

  for(gl = g_list_next(object_lists[WALL_INNERCIRCLE_T]); gl != NULL; gl = g_list_next(gl))
    ((WallInnerCircle*)gl->data)->draw_shape( false );
  for(gl = g_list_next(object_lists[WALL_LINE_T]); gl != NULL; gl = g_list_next(gl))
    ((WallLine*)gl->data)->draw_shape( false );

  for(gl = g_list_next(object_lists[WALL_CIRCLE_T]); gl != NULL; gl = g_list_next(gl))
    ((WallCircle*)gl->data)->draw_shape( false );



  for(gl = g_list_next(object_lists[MINE_T]); gl != NULL; gl = g_list_next(gl))
    if( ((Mine*)gl->data)->is_alive() )
      ((Mine*)gl->data)->draw_shape( false );

  for(gl = g_list_next(object_lists[COOKIE_T]); gl != NULL; gl = g_list_next(gl))
    if( ((Cookie*)gl->data)->is_alive() )
      ((Cookie*)gl->data)->draw_shape( false );

  draw_objects(false);
}

void
Gui::draw_circle( const Vector2D& center, const double radius, GdkColor& colour, const bool filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  double r;
  if( (r = radius*zoom) > 1.0 )
    {
      gdk_draw_arc (drawing_area->window,
                    colour_gc,
                    filled,
                    change_to_pixels_x(center[0]-radius),change_to_pixels_y(center[1]-radius),
                    (int)(r*2.0), (int)(r*2.0),
                    0, GDK_360_DEGREES);
    }
  else
    {
      gdk_draw_point (drawing_area->window,
                      colour_gc,
                      change_to_pixels_x(center[0]), 
                      change_to_pixels_y(center[1]));
    }
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
                 change_to_pixels_x(end[0]), change_to_pixels_y(end[1]));

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
Gui::set_score_window_title()
{
  String title = (String)"RealTimeBattle Score " + " Seq: " + the_arena.get_sequence_nr() + " Game: " +
    String(the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence()) +
    " Time: " + String((int)the_arena.get_total_time());
  gtk_window_set_title (GTK_WINDOW (score_window), title.chars());
}

void
Gui::change_selected_robot( const int row )
{
  GList* gl;
  Robot* robotp;

  for(gl = g_list_next(the_arena.get_object_lists()[ROBOT_T]); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)(gl->data);

      if( row == robotp->get_row_in_score_clist() )
        selected_robot = robotp;
    }
}

// This function will create the score window
void
Gui::setup_score_window()
{
  char * titles[6] = { "", "Name", "Energy ", "Place ", "Last ", "Score  " };

  score_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  set_score_window_title();
  gtk_widget_set_name (score_window, "RTB Score");
  gtk_signal_connect (GTK_OBJECT (score_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(score_window));
  gtk_container_border_width (GTK_CONTAINER (score_window), 12);
  gtk_widget_set_usize(score_window,
                       (int)the_opts.get_l(OPTION_SCORE_WINDOW_SIZE_X),(int)the_opts.get_l(OPTION_SCORE_WINDOW_SIZE_Y));
  gtk_widget_set_uposition(score_window,
                           (int)the_opts.get_l(OPTION_SCORE_WINDOW_POS_X),(int)the_opts.get_l(OPTION_SCORE_WINDOW_POS_Y));

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  GtkObject* hadj = gtk_adjustment_new ( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkObject* vadj = gtk_adjustment_new ( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkWidget* scrolled_win = gtk_scrolled_window_new (GTK_ADJUSTMENT ( hadj ),
                                                     GTK_ADJUSTMENT ( vadj ) );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_win ),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
  gtk_container_add (GTK_CONTAINER (score_window), scrolled_win);
  gtk_widget_show ( scrolled_win );
#endif

  score_clist = gtk_clist_new_with_titles( 6, titles);
  gtk_clist_set_selection_mode (GTK_CLIST(score_clist), GTK_SELECTION_BROWSE);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 0, 5);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 1, 120);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 2, 44);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 3, 38);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 4, 35);
  gtk_clist_set_column_width (GTK_CLIST(score_clist), 5, 45);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 0, GTK_JUSTIFY_CENTER);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 2, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 3, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 4, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(score_clist), 5, GTK_JUSTIFY_RIGHT);
  gtk_signal_connect(GTK_OBJECT(score_clist), "select_row",
                     GTK_SIGNAL_FUNC(new_robot_selected), NULL);
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_clist_set_shadow_type(GTK_CLIST(score_clist), GTK_SHADOW_IN);
  gtk_container_add(GTK_CONTAINER(scrolled_win), score_clist);
#else
  gtk_clist_set_border(GTK_CLIST(score_clist), GTK_SHADOW_IN);
  gtk_clist_set_policy(GTK_CLIST(score_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (score_window), score_clist);
#endif
  gtk_widget_show(score_clist);

  gtk_widget_show (score_window);
}

// This function resets score window and adds all robots in sequence to the score window
void
Gui::add_robots_to_score_list()
{                             
  gtk_clist_clear(GTK_CLIST(score_clist));
  GList** object_lists;
  GList* gl;
  Robot* robotp;

  object_lists = the_arena.get_object_lists();

  int robot_number=0;
  for(gl = g_list_next(object_lists[ROBOT_T]); gl != NULL; gl = g_list_next(gl))
    robot_number++;

  for(gl = g_list_next(object_lists[ROBOT_T]); gl != NULL; gl = g_list_next(gl))
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

      robotp->get_score_pixmap(score_window->window, colour_pixmap, bitmap_mask);

      gtk_clist_set_pixmap(GTK_CLIST(score_clist), row, 0, colour_pixmap, bitmap_mask);

      gtk_clist_set_text(GTK_CLIST(score_clist), row, 1, robotp->get_robot_name().non_const_chars());

      gtk_clist_set_text(GTK_CLIST(score_clist), row, 3, "");
      robotp->reset_last_displayed();
      robotp->display_score();

      for(int i=0; i<6; i++) delete [] list[i];
    }
  change_selected_robot( 0 );
}

// This function sets the title of the arena window
void
Gui::set_arena_window_title()
{
  String title = "RealTimeBattle Arena  "  + the_arena.get_current_arena_filename();
  gtk_window_set_title (GTK_WINDOW (arena_window), title.chars());
}

void
Gui::setup_arena_window()
{
  GtkWidget * vbox, * button_table;
  GtkWidget * button;

  // Window 

  arena_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  set_arena_window_title();
  gtk_widget_set_name (arena_window, "RTB Arena");
  gtk_widget_set_usize(arena_window,
                       (int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_X),(int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_Y));
  gtk_widget_set_uposition(arena_window,
                       (int)the_opts.get_l(OPTION_ARENA_WINDOW_POS_X),(int)the_opts.get_l(OPTION_ARENA_WINDOW_POS_Y));
  gtk_signal_connect (GTK_OBJECT (arena_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(arena_window));
  gtk_container_border_width (GTK_CONTAINER (arena_window), 12);  
 
  // VBox

  vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (arena_window), vbox);
  gtk_widget_show (vbox);

  // Zoom Buttons etc.

  zoomfactor = 1;

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
                                  GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
  gtk_container_add (GTK_CONTAINER (vbox), da_scrolled_window);
  gtk_widget_show (da_scrolled_window);

  // Drawing Area 

  drawing_area = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area),
                         (int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_X) - 48,(int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_Y) - 80);
  gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
                      (GtkSignalFunc) redraw_arena, (gpointer) NULL);
  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK);
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW( da_scrolled_window ), drawing_area );
#else
  gtk_container_add (GTK_CONTAINER (da_scrolled_window),drawing_area);
#endif
  gtk_widget_show (drawing_area);

  gtk_widget_show (arena_window);  

  // Background Colour 

  gdk_window_set_background (drawing_area->window, the_arena.get_background_colour_p());
  gdk_window_clear (drawing_area->window);
}

void
Gui::ask_user(String question, QuestionFunction function_name)
{
  question_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_window_set_title (GTK_WINDOW (question_window), "RealTimeBattle");
  gtk_widget_set_name (question_window, "RTB Question");
  gtk_window_set_policy(GTK_WINDOW (question_window), FALSE, FALSE, FALSE);
  gtk_window_position  (GTK_WINDOW (question_window), GTK_WIN_POS_CENTER);
  gtk_signal_connect (GTK_OBJECT (question_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(question_window));
  gtk_container_border_width (GTK_CONTAINER (question_window), 12);

  GtkWidget * vbox = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (question_window), vbox);
  gtk_widget_show (vbox);

  GtkWidget * label = gtk_label_new(question.chars());
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
  gtk_widget_show (label);

  GtkWidget * hbox = gtk_hbox_new (FALSE, 10);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
  gtk_widget_show (hbox);

  GtkWidget * button = gtk_button_new_with_label( "Yes" );
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (question_yes_callback), (gpointer) function_name);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  gtk_widget_show(button);

  button = gtk_button_new_with_label( "No" );
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (question_no_callback), (gpointer) function_name);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  gtk_widget_show(button);

  gtk_widget_show(question_window);
}

void
Gui::open_messagewindow()
{
  if( NULL == messagewindow_p )
    messagewindow_p = 
      new MessageWindow( the_opts.get_l( OPTION_MESSAGE_WINDOW_SIZE_X ),
                         the_opts.get_l( OPTION_MESSAGE_WINDOW_SIZE_Y ),
                         the_opts.get_l( OPTION_MESSAGE_WINDOW_POS_X ),
                         the_opts.get_l( OPTION_MESSAGE_WINDOW_POS_Y ) );
}

void
Gui::close_messagewindow()
{
  if( NULL != messagewindow_p )
    {
      delete messagewindow_p;
      messagewindow_p = NULL;
    }
}

void
Gui::close_score_window()
{
  gtk_widget_destroy ( score_window );
}

void
Gui::close_arena_window()
{
  gtk_widget_destroy ( arena_window );
}

void
Gui::close_question_window()
{
  gtk_widget_destroy ( question_window );
}
