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

#include "Gui.h"
#include "ArenaController.h"
#include "ArenaWindow.h"
#include "MessageWindow.h"
#include "ScoreWindow.h"
#include "StatisticsWindow.h"
#include "StartTournamentWindow.h"
#include "Dialog.h"
#include "List.h"
#include "Options.h"

Gui::Gui()
{
  arenawindow_p = NULL;
  messagewindow_p = NULL;
  scorewindow_p = NULL;
  statisticswindow_p = NULL;
  starttournamentwindow_p = NULL;
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
      {
        List<String> string_list;
        string_list.insert_last( new String( "Yes" ) );
        string_list.insert_last( new String( "No"  ) );
        Dialog( "This action will kill the current tournament.\nDo you want do that?",
                string_list, (DialogFunction) Gui::kill_and_start_new_tournament );
      }
  else
    close_starttournamentwindow();
}

void
Gui::kill_and_start_new_tournament( int result )
{
  if( result == 1 )
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
