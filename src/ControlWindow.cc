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
#include "ScoreWindow.h" 
#include "StatisticsWindow.h" 
#include "Gui.h"
#include "Dialog.h"
#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "Robot.h"
#include "Options.h"
#include "String.h"

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
                      (GtkSignalFunc) ControlWindow::quit_rtb,
                      (gpointer) NULL );

  // Main boxes

  GtkWidget* hbox = gtk_hbox_new ( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), hbox );
  gtk_widget_show( hbox );

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( hbox ), vbox );
  gtk_widget_show( vbox );

  // Buttons for all modes

  struct button_t { String label; GtkSignalFunc func; int pack; };

  struct button_t buttons[] = {
    { " New Tournament ", 
      (GtkSignalFunc) ControlWindow::new_tournament    , TRUE  },
    { " Pause ",
      (GtkSignalFunc) ControlWindow::pause             , TRUE  },
    { " End ",
      (GtkSignalFunc) ControlWindow::end_clicked       , TRUE  },
    { " Options ",
      (GtkSignalFunc) ControlWindow::options_clicked   , TRUE  },
    { " Statistics ",
      (GtkSignalFunc) ControlWindow::statistics_clicked, TRUE  },
    { "         Quit         ",
      (GtkSignalFunc) ControlWindow::quit_rtb          , FALSE } };

  GtkWidget* button_hbox = NULL;
  for(int i = 0;i < 6; i++)
    {
      if( i == 0 || i == 3 || i == 5 )
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

  gtk_widget_show( window_p );

  // Debug-mode buttons

  if( the_arena.get_game_mode() == ArenaBase::DEBUG_MODE )
    {
      GtkWidget* vseparator = gtk_vseparator_new();
      gtk_box_pack_start( GTK_BOX (hbox), vseparator, FALSE, FALSE, 0 );
      gtk_widget_show( vseparator );

      vbox = gtk_vbox_new( FALSE, 10 );
      gtk_container_add( GTK_CONTAINER( hbox ), vbox );
      gtk_widget_show( vbox );

      struct button_t debug_buttons[] = {
        { " Step ", 
          (GtkSignalFunc) ControlWindow::step      , TRUE  },
        { " End Game ", 
          (GtkSignalFunc) ControlWindow::end_game  , TRUE  },
        { " Kill Marked Robot ", 
          (GtkSignalFunc) ControlWindow::kill_robot, TRUE  } };

      for(int i = 0;i < 3; i++)
        {
          if( i == 0 || i == 2 )
            {
              button_hbox = gtk_hbox_new( FALSE, 10 );
              gtk_box_pack_start( GTK_BOX( vbox ), button_hbox,
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

      const char * arrow_xpms[2][13] = {
        {"9 10 2 1",
         "       c None",
         "x      c #000000000000",
         "       xx",
         "     xxxx",  
         "   xxxxxx",
         " xxxxxxxx",
         "xxxxxxxxx",
         "xxxxxxxxx",
         " xxxxxxxx",
         "   xxxxxx",
         "     xxxx",
         "       xx"},
        {"9 10 2 1",
         "       c None",
         "x      c #000000000000",
         "xx       ",
         "xxxx     ",  
         "xxxxxx   ",
         "xxxxxxxx ",
         "xxxxxxxxx",
         "xxxxxxxxx",
         "xxxxxxxx ",
         "xxxxxx   ",
         "xxxx     ",
         "xx       "}};

      button_hbox = gtk_hbox_new( FALSE, 10 );
      gtk_box_pack_start( GTK_BOX( vbox ), button_hbox,
                          FALSE, FALSE, 0);
      gtk_widget_show( button_hbox );

      GtkWidget* label = gtk_label_new( " Debug Level: " );
      gtk_box_pack_start( GTK_BOX( button_hbox ), label, TRUE, FALSE, 0 );
      gtk_widget_show( label );

      // Create pixmap and button for left arrow
      {
        GdkPixmap* pixmap;
        GdkBitmap* bitmap_mask;
        pixmap = gdk_pixmap_create_from_xpm_d( window_p->window,
                                               &bitmap_mask,
                                               &(window_p->style->black),
                                               (gchar **)arrow_xpms[0] );
        GtkWidget* pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
        gtk_widget_show( pixmap_widget );
        GtkWidget* button = gtk_button_new();
        gtk_container_add( GTK_CONTAINER( button ), pixmap_widget );
        gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                            (GtkSignalFunc) decrease_debug_level,
                            (gpointer) this );
        gtk_widget_set_usize( button, 22, 20 );
        gtk_box_pack_start( GTK_BOX( button_hbox ), button, TRUE, FALSE, 0 );
        gtk_widget_show( button );
      }

      // Debug level
      debug_level_label =
        gtk_label_new( String( the_arena.get_debug_level() ).chars() );
      gtk_box_pack_start( GTK_BOX( button_hbox ),
                          debug_level_label, TRUE, FALSE, 0 );
      gtk_widget_show( debug_level_label );

      // Create pixmap and button for right arrow
      {
        GdkPixmap* pixmap;
        GdkBitmap* bitmap_mask;
        pixmap = gdk_pixmap_create_from_xpm_d( window_p->window,
                                               &bitmap_mask,
                                               &(window_p->style->black),
                                               (gchar **)arrow_xpms[1] );
        GtkWidget* pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
        gtk_widget_show( pixmap_widget );
        GtkWidget* button = gtk_button_new();
        gtk_container_add( GTK_CONTAINER( button ), pixmap_widget );
        gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                            (GtkSignalFunc) increase_debug_level,
                            (gpointer) this );
        gtk_widget_set_usize( button, 22, 20 );
        gtk_box_pack_start( GTK_BOX( button_hbox ), button, TRUE, FALSE, 0 );
        gtk_widget_show( button );
      }
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
ControlWindow::quit_rtb( GtkWidget* widget, gpointer data )
{
  Quit();
}

void
ControlWindow::pause( GtkWidget* widget, class ControlWindow* controlwindow_p )
{
  the_arena.pause_game_toggle();
}

void
ControlWindow::step( GtkWidget* widget, gpointer data )
{
  the_arena.step_paused_game();
}

void
ControlWindow::end_game( GtkWidget* widget, gpointer data )
{
  if( the_arena.get_state() != NOT_STARTED &&
      the_arena.get_state() != FINISHED )
    the_arena.end_game();
}

void
ControlWindow::kill_robot( GtkWidget* widget, gpointer data )
{
  if(the_arena.get_state() == GAME_IN_PROGRESS || 
     the_arena.get_state() == PAUSED )
    {
      Robot* robotp = the_gui.get_scorewindow_p()->get_selected_robot();
      if( robotp != NULL )
        robotp->die();
    }
}

void
ControlWindow::decrease_debug_level( GtkWidget* widget,
                                     class ControlWindow* controlwindow_p )
{
  the_arena.set_debug_level( the_arena.get_debug_level() - 1);
  gtk_label_set( GTK_LABEL( controlwindow_p->get_debug_level_label() ),
                 String( the_arena.get_debug_level() ).chars() );  
}

void
ControlWindow::increase_debug_level( GtkWidget* widget,
                                     class ControlWindow* controlwindow_p )
{
  the_arena.set_debug_level( the_arena.get_debug_level() + 1);
  gtk_label_set( GTK_LABEL( controlwindow_p->get_debug_level_label() ),
                 String( the_arena.get_debug_level() ).chars() );  
}

void
ControlWindow::new_tournament( GtkWidget* widget,
                               class ControlWindow* controlwindow_p )
{
  the_gui.open_starttournamentwindow();
}

void
ControlWindow::end_clicked( GtkWidget* widget, gpointer data )
{
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
  if( result == 1 )
    the_arena.interrupt_tournament();
}

void
ControlWindow::options_clicked( GtkWidget* widget,
                                class ControlWindow* controlwindow_p )
{
  the_opts.open_optionswindow();
}

void
ControlWindow::statistics_clicked( GtkWidget* widget,
                                   class ControlWindow* controlwindow_p )
{
  the_gui.open_statisticswindow();
}
