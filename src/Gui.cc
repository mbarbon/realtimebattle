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

#include "ArenaWindow.h"
#include "MessageWindow.h"
#include "ScoreWindow.h"
#include "StatisticsWindow.h"
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

Gui::Gui()
{
  arenawindow_p = NULL;
  messagewindow_p = NULL;
  scorewindow_p = NULL;
  statisticswindow_p = NULL;
  starttournamentwindow_p = NULL;
}

void
Gui::quit_event()
{
  gtk_main_quit();
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
Gui::open_arenawindow()
{
  if( NULL == arenawindow_p )
    arenawindow_p = 
      new ArenaWindow( the_opts.get_l( OPTION_ARENA_WINDOW_SIZE_X ),
                       the_opts.get_l( OPTION_ARENA_WINDOW_SIZE_Y ),
                       the_opts.get_l( OPTION_ARENA_WINDOW_POS_X  ),
                       the_opts.get_l( OPTION_ARENA_WINDOW_POS_Y  ) );
}

void
Gui::close_arenawindow()
{
  if( NULL != arenawindow_p )
    {
      delete arenawindow_p;
      arenawindow_p = NULL;
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
Gui::open_statisticswindow()
{
  if( NULL == statisticswindow_p && the_arena.get_state() != NOT_STARTED )
    statisticswindow_p = 
      new StatisticsWindow( the_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_X ),
                            the_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_Y ),
                            -1, -1 );
}

void
Gui::close_statisticswindow()
{
  if( NULL != statisticswindow_p )
    {
      delete statisticswindow_p;
      statisticswindow_p = NULL;
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
Gui::close_question_window()
{
  gtk_widget_destroy ( question_window );
}
