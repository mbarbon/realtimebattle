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

#include <gtk/gtk.h>

#include "ControlWindow.h"
#include "ArenaWindow.h"
#include "MessageWindow.h"
#include "ScoreWindow.h" 
#include "StatisticsWindow.h" 
#include "Gui.h"
#include "Dialog.h"
#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "ArenaReplay.h"
#include "Robot.h"
#include "Options.h"
#include "String.h"

extern class ControlWindow* controlwindow_p;

ControlWindow::ControlWindow( const int default_width,
                              const int default_height,
                              const int default_x_pos,
                              const int default_y_pos )
{
  // The window widget

  window_p = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_widget_set_name( window_p, "RTB Control" );
  gtk_window_set_policy( GTK_WINDOW( window_p ), FALSE, FALSE, FALSE );

  set_window_title( "RealTimeBattle" );

  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  if( default_width != -1 && default_height != -1 )
    gtk_widget_set_usize( window_p, default_width, default_height );
  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );

  gtk_signal_connect( GTK_OBJECT( window_p ), "delete_event",
                      (GtkSignalFunc) ControlWindow::delete_event_occured,
                      (gpointer) this );

  // Main boxes

  window_hbox = gtk_hbox_new ( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), window_hbox );
  gtk_widget_show( window_hbox );

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_hbox ), vbox );
  gtk_widget_show( vbox );

  // Buttons for all modes

  struct button_t { String label; GtkSignalFunc func; int pack; };

  struct button_t buttons[] = {
    { (String)" New Tournament ", 
      (GtkSignalFunc) ControlWindow::new_tournament    , TRUE  },
    { (String)" Replay Tournament ", 
      (GtkSignalFunc) ControlWindow::replay_tournament , TRUE  },
    { (String)" Options ",
      (GtkSignalFunc) ControlWindow::options_clicked   , TRUE  },
    { (String)" Statistics ",
      (GtkSignalFunc) ControlWindow::statistics_clicked, TRUE  },
    { (String)" Pause ",
      (GtkSignalFunc) ControlWindow::pause             , TRUE  },
    { (String)" End ",
      (GtkSignalFunc) ControlWindow::end_clicked       , TRUE  },
    { (String)"         Quit         ",
      (GtkSignalFunc) ControlWindow::quit_rtb          , FALSE } };

  GtkWidget* button_hbox = NULL;
  for(int i = 0;i < 7; i++)
    {
      if( i == 0 || i == 2 || i == 6 )
        {
          button_hbox = gtk_hbox_new( FALSE, 10 );
          gtk_box_pack_start( GTK_BOX( vbox ), button_hbox,
                              FALSE, FALSE, 0);
          gtk_widget_show( button_hbox );
        }
      GtkWidget* button =
        gtk_button_new_with_label( buttons[i].label.chars() );
      gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                          (GtkSignalFunc) buttons[i].func,
                          (gpointer) this );
      gtk_box_pack_start( GTK_BOX( button_hbox ), button,
                          TRUE, buttons[i].pack , 0 );
      gtk_widget_show( button );
    }

  GtkWidget* vsep = gtk_vseparator_new();
  gtk_box_pack_start( GTK_BOX (window_hbox), vsep, FALSE, FALSE, 0 );
  gtk_widget_show( vsep );

  GtkWidget* win_control_vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_hbox ), win_control_vbox );
  gtk_widget_show( win_control_vbox );

  struct button_t check_buttons[] = {
    { (String)" Show arena window ",
      (GtkSignalFunc) ControlWindow::arena_window_toggle, TRUE  },
    { (String)" Show message window ",
      (GtkSignalFunc) ControlWindow::message_window_toggle, TRUE  },
    { (String)" Show score window ",
      (GtkSignalFunc) ControlWindow::score_window_toggle, TRUE  } };
  
  for( int i = 0;i < 3; i++ )
    {
      GtkWidget* checkbutton =
        gtk_check_button_new_with_label( check_buttons[i].label.chars() );
      gtk_box_pack_start( GTK_BOX( win_control_vbox ), checkbutton, TRUE, TRUE, 0 );
      gtk_signal_connect( GTK_OBJECT( checkbutton ), "toggled",
                          check_buttons[i].func, (gpointer) this );
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
      gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( checkbutton ), TRUE );
#else
      gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON( checkbutton ), TRUE );
#endif
      gtk_widget_show( checkbutton );

      switch( i )
        {
        case 0:
          show_arena_checkbutton = checkbutton;
          break;
        case 1:
          show_message_checkbutton = checkbutton;
          break;
        case 2:
          show_score_checkbutton = checkbutton;
          break;
        }
    }

  gtk_widget_show( window_p );
  vseparator = NULL;
  extra_vbox = NULL;
  filesel = NULL;

  remove_replay_widgets();
}

void
ControlWindow::remove_replay_widgets()
{
  if( the_arena_controller.game_mode == ArenaBase::DEBUG_MODE )
    display_debug_widgets();
  else
    clear_extra_widgets();
}

void
ControlWindow::clear_extra_widgets()
{
  if( extra_vbox != NULL ) gtk_widget_destroy( extra_vbox );
  if( vseparator != NULL ) gtk_widget_destroy( vseparator );

  extra_vbox = NULL;
  vseparator = NULL;

  displayed = NO_WIDGETS;
}

void
ControlWindow::display_debug_widgets()
{
  clear_extra_widgets();
  
  vseparator = gtk_vseparator_new();
  gtk_box_pack_start( GTK_BOX (window_hbox), vseparator, FALSE, FALSE, 0 );
  gtk_widget_show( vseparator );

  extra_vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_hbox ), extra_vbox );
  gtk_widget_show( extra_vbox );

  struct button_t { String label; GtkSignalFunc func; int pack; };
  struct button_t debug_buttons[] = {
    { (String)" Step ", 
      (GtkSignalFunc) ControlWindow::step      , TRUE  },
    { (String)" End Game ", 
      (GtkSignalFunc) ControlWindow::end_game  , TRUE  },
    { (String)" Kill Marked Robot ", 
      (GtkSignalFunc) ControlWindow::kill_robot, TRUE  } };

  GtkWidget* button_hbox = NULL;
  for(int i = 0;i < 3; i++)
    {
      if( i == 0 || i == 2 )
        {
          button_hbox = gtk_hbox_new( FALSE, 10 );
          gtk_box_pack_start( GTK_BOX( extra_vbox ), button_hbox,
                              FALSE, FALSE, 0);
          gtk_widget_show( button_hbox );
        }
      GtkWidget* button = 
        gtk_button_new_with_label( debug_buttons[i].label.chars() );
      gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                          (GtkSignalFunc) debug_buttons[i].func,
                          (gpointer) NULL );
      gtk_box_pack_start( GTK_BOX( button_hbox ), button,
                          TRUE, debug_buttons[i].pack , 0);
      gtk_widget_show( button );
    }

  button_hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX( extra_vbox ), button_hbox,
                      FALSE, FALSE, 0);
  gtk_widget_show( button_hbox );

  GtkWidget* label = gtk_label_new( " Debug Level: " );
  gtk_box_pack_start( GTK_BOX( button_hbox ), label, TRUE, FALSE, 0 );
  gtk_widget_show( label );

  GtkAdjustment* adj =
    (GtkAdjustment*) gtk_adjustment_new( the_arena_controller.debug_level, 0,
                                         max_debug_level, 1, 1, 0 );

  debug_level = gtk_spin_button_new( adj, 0, 0 );
  gtk_signal_connect( GTK_OBJECT( adj ), "value_changed",
                      (GtkSignalFunc) change_debug_level,
                      (gpointer) this );
  gtk_box_pack_start( GTK_BOX( button_hbox ), debug_level, TRUE, FALSE, 0 );
  gtk_widget_show( debug_level );

  displayed = DEBUG_WIDGETS;
}

void
ControlWindow::display_replay_widgets()
{
  clear_extra_widgets();

  vseparator = gtk_vseparator_new();
  gtk_box_pack_start( GTK_BOX (window_hbox), vseparator, FALSE, FALSE, 0 );
  gtk_widget_show( vseparator );

  extra_vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_hbox ), extra_vbox );
  gtk_widget_show( extra_vbox );

  GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX( extra_vbox ), hbox, FALSE, FALSE, 0 );
  gtk_widget_show( hbox );

  current_replay_time_adjustment =
    (GtkAdjustment*) gtk_adjustment_new ( 0.0, 0.0,
                                          replay_arena.get_length_of_current_game(),
                                          0.1, 1.0, 1.0 );

  gtk_signal_connect( GTK_OBJECT( current_replay_time_adjustment ), "value_changed",
                      (GtkSignalFunc) change_current_replay_time,
                      (gpointer) this );

  time_control =
    gtk_hscale_new( GTK_ADJUSTMENT( current_replay_time_adjustment ) );
  gtk_widget_set_usize( GTK_WIDGET( time_control ), 150, 30 );
  gtk_range_set_update_policy( GTK_RANGE( time_control ), GTK_UPDATE_DELAYED );
  gtk_scale_set_digits( GTK_SCALE( time_control ), 1 );
  gtk_scale_set_draw_value( GTK_SCALE( time_control ), TRUE );
  gtk_box_pack_start( GTK_BOX( hbox ), time_control, TRUE, TRUE, 0 );
  gtk_widget_show( time_control );

  char* rew_xpm[13] =
  { "18 10 2 1",
    "       c None",
    "x      c #000000000000",
    "       xx       xx",
    "     xxxx     xxxx",  
    "   xxxxxx   xxxxxx",
    " xxxxxxxx xxxxxxxx",
    "xxxxxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxxxxx",
    " xxxxxxxx xxxxxxxx",
    "   xxxxxx   xxxxxx",
    "     xxxx     xxxx",
    "       xx       xx" };
  char* ffw_xpm[13] =
  { "18 10 2 1",
    "       c None",
    "x      c #000000000000",
    "xx       xx       ",
    "xxxx     xxxx     ",  
    "xxxxxx   xxxxxx   ",
    "xxxxxxxx xxxxxxxx ",
    "xxxxxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxxxxx",
    "xxxxxxxx xxxxxxxx ",
    "xxxxxx   xxxxxx   ",
    "xxxx     xxxx     ",
    "xx       xx       " };

  struct button_t
  {
    char** xpm;
    String label;
    GtkSignalFunc clicked_func;
    GtkSignalFunc pressed_func;
    GtkSignalFunc released_func;
  };

  struct button_t replay_buttons[] = {
    { rew_xpm, (String)"",
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::rewind_pressed,
      (GtkSignalFunc) ControlWindow::rewind_released },
    { ffw_xpm, (String)"", 
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::fast_forward_pressed,
      (GtkSignalFunc) ControlWindow::fast_forward_released },
    { NULL, (String)" Step forward ", 
      (GtkSignalFunc) ControlWindow::step_forward,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (String)" Step backward ", 
      (GtkSignalFunc) ControlWindow::step_backward,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (String)" Next Game ", 
      (GtkSignalFunc) ControlWindow::next_game,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (String)" Prev Game ", 
      (GtkSignalFunc) ControlWindow::prev_game,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (String)" Next Seq ", 
      (GtkSignalFunc) ControlWindow::next_seq,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (String)" Prev Seq ", 
      (GtkSignalFunc) ControlWindow::prev_seq,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy } };

  GtkWidget* button_hbox = NULL;

  for(int i = 0;i < 8; i++)
    {
      if( i == 0 || i == 4 )
        {
          button_hbox = gtk_hbox_new( FALSE, 10 );
          gtk_box_pack_start( GTK_BOX( extra_vbox ), button_hbox,
                              FALSE, FALSE, 0);
          gtk_widget_show( button_hbox );
        }
      GtkWidget* button_w;
      if( replay_buttons[i].xpm != NULL )
        {
          button_w = gtk_button_new();
          GdkPixmap* pixmap;
          GdkBitmap* bitmap_mask;

          pixmap = gdk_pixmap_create_from_xpm_d( window_p->window,
                                                 &bitmap_mask,
                                                 &(window_p->style->black),
                                                 replay_buttons[i].xpm );
          GtkWidget* pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
          gtk_widget_show( pixmap_widget );
          gtk_container_add( GTK_CONTAINER( button_w ), pixmap_widget );
          gtk_widget_set_usize( button_w, 32, 20 );
        }
      else
        button_w = 
          gtk_button_new_with_label( replay_buttons[i].label.chars() );

      gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
                          (GtkSignalFunc) replay_buttons[i].clicked_func,
                          (gpointer) NULL );
      gtk_signal_connect( GTK_OBJECT( button_w ), "pressed",
                          (GtkSignalFunc) replay_buttons[i].pressed_func,
                          (gpointer) NULL );
      gtk_signal_connect( GTK_OBJECT( button_w ), "released",
                          (GtkSignalFunc) replay_buttons[i].released_func,
                          (gpointer) NULL );
      gtk_box_pack_start( GTK_BOX( button_hbox ), button_w,
                          TRUE, TRUE , 0);
      gtk_widget_show( button_w );
    }

  displayed = REPLAY_WIDGETS;
}

void
ControlWindow::change_time_limitations()
{
  if( displayed == REPLAY_WIDGETS )
    {
      // Possible memory leak: Do not know how to destroy old adjustment
      // possibly destroyed by gtk_range_set_adjustment()
      current_replay_time_adjustment =
        (GtkAdjustment*) gtk_adjustment_new ( 0.0, 0.0,
                                              replay_arena.get_length_of_current_game(),
                                              0.1, 1.0, 1.0 );
      gtk_signal_connect( GTK_OBJECT( current_replay_time_adjustment ), "value_changed",
                          (GtkSignalFunc) change_current_replay_time,
                          (gpointer) this );
      gtk_range_set_adjustment( GTK_RANGE( time_control ),
                                current_replay_time_adjustment );
    }
}

ControlWindow::~ControlWindow()
{
  gtk_widget_destroy( window_p );
}

void
ControlWindow::set_window_title( const String& text)
{
  String title = text;
  gtk_window_set_title( GTK_WINDOW( window_p ), title.chars() );
}

void
ControlWindow::delete_event_occured( GtkWidget* widget, GdkEvent* event,
                                     class ControlWindow* cw_p )
{
  Quit();
}

void
ControlWindow::quit_rtb( GtkWidget* widget,
                         class ControlWindow* cw_p )
{
  Quit();
}

void
ControlWindow::pause( GtkWidget* widget, class ControlWindow* cw_p )
{
  if( the_arena_controller.is_started() )
    the_arena.pause_game_toggle();
}

void
ControlWindow::step( GtkWidget* widget, gpointer data )
{
  if( the_arena_controller.is_started() )
    the_arena.step_paused_game();
}

void
ControlWindow::end_game( GtkWidget* widget, gpointer data )
{
  if( the_arena_controller.is_started() )
    if( the_arena.get_state() != NOT_STARTED &&
        the_arena.get_state() != FINISHED )
      the_arena.end_game();
}

void
ControlWindow::kill_robot( GtkWidget* widget, gpointer data )
{
  if( the_arena_controller.is_started() )
    if(the_arena.get_state() == GAME_IN_PROGRESS || 
       the_arena.get_state() == PAUSED )
      {
        Robot* robotp = the_gui.get_scorewindow_p()->get_selected_robot();
        if( robotp != NULL )
          robotp->die();
      }
}

void
ControlWindow::change_debug_level( GtkAdjustment *adj,
                                   class ControlWindow* cw_p )
{
  if( the_arena_controller.is_started() )
    the_arena.set_debug_level
      ( gtk_spin_button_get_value_as_int
        ( GTK_SPIN_BUTTON( cw_p->debug_level ) ) );
}

void
ControlWindow::new_tournament( GtkWidget* widget,
                               class ControlWindow* cw_p )
{
  the_gui.open_starttournamentwindow();
}

void
ControlWindow::replay_tournament( GtkWidget* widget,
                                  class ControlWindow* cw_p )
{
  bool open = false;

  if( the_arena_controller.is_started() )
    if( the_arena.get_state() != NOT_STARTED &&
        the_arena.get_state() != FINISHED )
      {
        List<String> string_list;
        string_list.insert_last( new String( "Yes" ) );
        string_list.insert_last( new String( "No"  ) );
        Dialog( (String)"This action will kill the current tournament.\n" +
                "Do you want do that?",
                string_list,
                (DialogFunction) ControlWindow::kill_and_open_filesel );
      }
    else
      open = true;
  else
    open = true;

  if( open )
    cw_p->open_replay_filesel();
}

void
ControlWindow::open_replay_filesel()
{
  if( filesel == NULL )
    {
      filesel = gtk_file_selection_new( "Choose a log file to replay" );
      gtk_signal_connect( GTK_OBJECT( filesel ), "destroy",
                          (GtkSignalFunc) ControlWindow::destroy_filesel,
                          (gpointer) this );
      gtk_signal_connect
        ( GTK_OBJECT( GTK_FILE_SELECTION( filesel )->cancel_button ), "clicked",
          (GtkSignalFunc) ControlWindow::destroy_filesel, (gpointer) this );
      gtk_signal_connect
        ( GTK_OBJECT( GTK_FILE_SELECTION( filesel )->ok_button ), "clicked",
          (GtkSignalFunc) ControlWindow::replay, (gpointer) this );
      gtk_widget_show( filesel );
    }
}

void
ControlWindow::kill_and_open_filesel( int result )
{
  if( the_arena_controller.is_started() && result == 1 )
    {
      the_arena.interrupt_tournament();
      controlwindow_p->open_replay_filesel();
    }
}


void
ControlWindow::arena_window_toggle( GtkWidget* widget,
                                    class ControlWindow* cw_p )
{
  bool active = GTK_TOGGLE_BUTTON( widget )->active;

  if( the_gui.is_arenawindow_up() )
    {
      if( active )
        the_gui.get_arenawindow_p()->
          show_window( the_gui.get_arenawindow_p()->get_window_p(),
                       the_gui.get_arenawindow_p() );
      else
        the_gui.get_arenawindow_p()->
          hide_window( the_gui.get_arenawindow_p()->get_window_p(),
                       NULL, the_gui.get_arenawindow_p() );
    }
}

bool
ControlWindow::is_arenawindow_checked()
{
  return GTK_TOGGLE_BUTTON( show_arena_checkbutton )->active;
}

void
ControlWindow::message_window_toggle( GtkWidget* widget,
                                      class ControlWindow* cw_p )
{
  bool active = GTK_TOGGLE_BUTTON( widget )->active;

  if( the_gui.is_messagewindow_up() )
    {
      if( active )
        the_gui.get_messagewindow_p()->
          show_window( the_gui.get_messagewindow_p()->get_window_p(),
                       the_gui.get_messagewindow_p() );
      else
        the_gui.get_messagewindow_p()->
          hide_window( the_gui.get_messagewindow_p()->get_window_p(),
                       NULL, the_gui.get_messagewindow_p() );
    }
}

bool
ControlWindow::is_messagewindow_checked()
{
  return GTK_TOGGLE_BUTTON( show_message_checkbutton )->active;
}

void
ControlWindow::score_window_toggle( GtkWidget* widget,
                                    class ControlWindow* cw_p )
{
  bool active = GTK_TOGGLE_BUTTON( widget )->active;

  if( the_gui.is_scorewindow_up() )
    {
      if( active )
        the_gui.get_scorewindow_p()->
          show_window( the_gui.get_scorewindow_p()->get_window_p(),
                       the_gui.get_scorewindow_p() );
      else
        the_gui.get_scorewindow_p()->
          hide_window( the_gui.get_scorewindow_p()->get_window_p(),
                       NULL, the_gui.get_scorewindow_p() );
    }
}

bool
ControlWindow::is_scorewindow_checked()
{
  return GTK_TOGGLE_BUTTON( show_score_checkbutton )->active;
}

void
ControlWindow::replay( GtkWidget* widget,
                       class ControlWindow* cw_p )
{
  the_arena_controller.replay_filename =
    gtk_file_selection_get_filename
    ( GTK_FILE_SELECTION( cw_p->get_filesel() ) );
  the_arena_controller.start_replay_arena();
  destroy_filesel( cw_p->get_filesel(), cw_p );
}

void
ControlWindow::destroy_filesel( GtkWidget* widget,
                                class ControlWindow* cw_p )
{
  gtk_widget_destroy( cw_p->get_filesel() );
  cw_p->set_filesel( NULL );
}

void
ControlWindow::end_clicked( GtkWidget* widget, gpointer data )
{
  if( the_arena_controller.is_started() )
    if( the_arena.get_state() != NOT_STARTED &&
        the_arena.get_state() != FINISHED )
      {
        List<String> string_list;
        string_list.insert_last( new String( "Yes" ) );
        string_list.insert_last( new String( "No"  ) );
        Dialog( "This action will kill the current tournament.\nDo you want do that?",
                string_list, (DialogFunction) ControlWindow::end_tournament );
      }
}

void
ControlWindow::end_tournament( int result )
{
  if( the_arena_controller.is_started() && result == 1 )
    the_arena.interrupt_tournament();
}

void
ControlWindow::options_clicked( GtkWidget* widget,
                                class ControlWindow* cw_p )
{
  the_opts.open_optionswindow();
}

void
ControlWindow::statistics_clicked( GtkWidget* widget,
                                   class ControlWindow* cw_p )
{
  the_gui.open_statisticswindow();
}

void
ControlWindow::rewind_pressed( GtkWidget* widget,
                               class ControlWindow* cw_p )
{
  cout << "rewinding ... " << endl;
  replay_arena.change_speed( false, true );
}

void
ControlWindow::rewind_released( GtkWidget* widget,
                                class ControlWindow* cw_p )
{
  cout << "until released" << endl;
  replay_arena.change_speed( false, false );
}

void
ControlWindow::fast_forward_pressed( GtkWidget* widget,
                                     class ControlWindow* cw_p )
{
  cout << "forwarding ... " << endl;
  replay_arena.change_speed( true, true );
}

void
ControlWindow::fast_forward_released( GtkWidget* widget,
                                      class ControlWindow* cw_p )
{
  cout << "until released" << endl;
  replay_arena.change_speed( true, false );
}

void
ControlWindow::step_forward( GtkWidget* widget,
                             class ControlWindow* cw_p )
{
  cout << "Stepping forward" << endl;
  replay_arena.step_forward( 1 );  
}

void
ControlWindow::step_backward( GtkWidget* widget,
                              class ControlWindow* cw_p )
{
  cout << "Stepping backward" << endl;
  replay_arena.step_forward( -1 );  
}

void
ControlWindow::next_game( GtkWidget* widget,
                          class ControlWindow* cw_p )
{
  replay_arena.change_game( 1, 0 );
}

void
ControlWindow::prev_game( GtkWidget* widget,
                          class ControlWindow* cw_p )
{
  replay_arena.change_game( -1, 0 );
}

void
ControlWindow::next_seq( GtkWidget* widget,
                         class ControlWindow* cw_p )
{
  replay_arena.change_game( 0, 1 );
}

void
ControlWindow::prev_seq( GtkWidget* widget,
                         class ControlWindow* cw_p )
{
  replay_arena.change_game( 0, -1 );
}

void
ControlWindow::change_current_replay_time( GtkAdjustment *adj,
                                           class ControlWindow* cw_p )
{
  replay_arena.change_replay_time( adj->value );
}

void
ControlWindow::set_progress_time( const double time )
{
  gtk_adjustment_set_value( current_replay_time_adjustment, time );
}
