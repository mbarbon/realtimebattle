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

#include "Arena_Controller.h"
#include "Arena_RealTime.h"

#include "MessageWindow.h"
#include "ScoreWindow.h"
#include "StartTournamentWindow.h"

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
  List<Shape>* object_lists;
  Robot* robotp;

  if((da_scrolled_window->allocation.width - 24 !=  da_scrolled_window_size[0]) ||
     (da_scrolled_window->allocation.height - 24 !=  da_scrolled_window_size[1]))
    change_zoom();

  object_lists = the_arena.get_object_lists();

  ListIterator<Shape> li;
  for( object_lists[SHOT_T].first(li); li.ok(); li++ )
    if( ((Shot*)li())->is_alive() )
      ((Shot*)li())->draw_shape( clear_objects_first );


  ListIterator<Shape> li2;
  for( object_lists[ROBOT_T].first(li2); li2.ok(); li2++ )
    {
      robotp = (Robot*)li2();

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
  List<Shape>* object_lists;

  object_lists = the_arena.get_object_lists();
  ListIterator<Shape> li;

  // Must begin with innercircles (the are destructive)
  for( int obj_type=WALL_INNERCIRCLE_T; obj_type > SHOT_T; obj_type--) 
    {
      for( object_lists[obj_type].first(li); li.ok(); li++ )
        {
          if( !( ( obj_type == MINE_T || obj_type == COOKIE_T ) &&
                 ((Extras*)li())->is_alive())  )
            {
              li()->draw_shape( false );
            }
        }
    }

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
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_window_set_default_size( GTK_WINDOW( arena_window ),
                               (int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_X),
                               (int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_Y) );
  gtk_widget_set_usize( arena_window, 185, 120 );
#else
  gtk_widget_set_usize(arena_window,
                       (int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_X),
                       (int)the_opts.get_l(OPTION_ARENA_WINDOW_SIZE_Y));
#endif
  gtk_widget_set_uposition(arena_window,
                           (int)the_opts.get_l(OPTION_ARENA_WINDOW_POS_X),
                           (int)the_opts.get_l(OPTION_ARENA_WINDOW_POS_Y));
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
Gui::open_scorewindow()
{
  if( NULL == scorewindow_p )
    scorewindow_p = 
      new ScoreWindow( the_opts.get_l( OPTION_SCORE_WINDOW_SIZE_X ),
                       the_opts.get_l( OPTION_SCORE_WINDOW_SIZE_Y ),
                       the_opts.get_l( OPTION_SCORE_WINDOW_POS_X ),
                       the_opts.get_l( OPTION_SCORE_WINDOW_POS_Y ) );
}

void
Gui::close_scorewindow()
{
  if( NULL != scorewindow_p )
    {
      delete scorewindow_p;
      scorewindow_p = NULL;
    }
}

void
Gui::open_starttournamentwindow()
{
  if( NULL == starttournamentwindow_p )
    if( the_arena.get_state() == NOT_STARTED || 
        the_arena.get_state() == FINISHED )
      starttournamentwindow_p = 
        new StartTournamentWindow( -1, -1, -1, -1 );
    else
      the_gui.ask_user( "This action will kill the current tournament.\nDo you want do that?",
                       &(Gui::kill_and_start_new_tournament) );
  else
    close_starttournamentwindow();
}

void
Gui::kill_and_start_new_tournament( bool really )
{
  if( really )
    {
      the_arena.interrupt_tournament();
      the_gui.open_starttournamentwindow();
    }
}

void
Gui::close_starttournamentwindow()
{
  if( NULL != starttournamentwindow_p )
    {
      delete starttournamentwindow_p;
      starttournamentwindow_p = NULL;
    }
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
Gui::close_arena_window()
{
  gtk_widget_destroy ( arena_window );
}

void
Gui::close_question_window()
{
  gtk_widget_destroy ( question_window );
}
