/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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
# include <config.h>
#endif
#include <stdlib.h>

#include <typeinfo>
#include <string>

#include "Gui.h"
#include "GuiInterface.h"
#include "IntlDefs.h"
#include "ArenaWindow.h"
#include "MessageWindow.h"
#include "OptionsWindow.h"
#include "ScoreWindow.h"
#include "ControlWindow.h"
#include "StatisticsWindow.h"
#include "StartTournamentWindow.h"
#include "Dialog.h"
#include "DrawingObjects.h"
#include "Option.h"
#include "OptionHandler.h"
#include "GuiVarious.h"
#include "Various.h"
#include "InfoClasses.h"

class Gui* gui_p;

const string
GIName()
{
  static const string name(_("Gtk+ based GUI"));
  return name;
}

const string
GIUsageMessage()
{
  static const string usage(_("No options"));
  return usage;
}

bool
GIInit( int argc, char** argv )
{
#ifdef HAVE_LOCALE_H
  gtk_set_locale();
#endif

  gtk_init (&argc, &argv);

  gui_p = new Gui();
  return (gui_p != NULL);
}
//TODO: How do we get the_opts?
int
GIMain( GuiClientInterface* _gi_p )
{
  int returncode = the_gui.main_loop( _gi_p );
  _gi_p->quit( true );
  return returncode;
}

Gui::Gui()
{
  arenawindow_p = NULL;
  messagewindow_p = NULL;
  scorewindow_p = NULL;
  statisticswindow_p = NULL;
  starttournamentwindow_p = NULL;

  initialize_gtk_options();
}

Gui::~Gui()
{
  close_statisticswindow();
  close_starttournamentwindow();
  close_scorewindow();
  close_messagewindow();
  close_arenawindow();

  delete controlwindow_p;

  for( int i = ROBOT; i < LAST_OBJECT_TYPE; i++ )
    clear_and_delete_pointer_list( drawing_objects_lists[i] );
}

void
Gui::initialize_gtk_options()
{
  map<string,Option*> all_options;
  vector<string> group_names;

  group_names.push_back( _("Window sizes") );

  const int GROUP_SIZE_OF_WINDOWS = 0;

  // ---------- Size of Windows ----------

  // TODO: There's something very strange with the Optin (occurs only in Gui)
  //       Newly created Option-objects does not contain the values they should.

  all_options["Arena window xsize"] = (Option*) new
    LongOption( GROUP_SIZE_OF_WINDOWS, 350, 185, 10000, false, false,
                _("Initial Arena window width") );

  all_options["Arena window ysize"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 350, 120, 10000, false, false,
                _("Initial Arena window height") );

  all_options["Arena window xpos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Arena window x position") );

  all_options["Arena window ypos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Arena window y position") );

  all_options["Control window xpos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Control window x position") );

  all_options["Control window ypos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Control window y position") );

  all_options["Message window xsize"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 394, 300, 10000, false, false,
                _("Initial Message window width") );

  all_options["Message window ysize"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 210, 110, 10000, false, false,
                _("Initial Message window height") );

  all_options["Message window xpos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Message window x position") );

  all_options["Message window ypos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Message window y position") );

  all_options["Score window xsize"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 394, 175, 10000, false, false,
                _("Initial Score window width") );

  all_options["Score window ysize"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 374, 80, 10000, false, false,
                _("Initial Score window height") );

  all_options["Score window xpos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Score window x position") );

  all_options["Score window ypos"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                _("Initial Score window y position") );

  all_options["Statistics window xsize"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 499, 320, 10000, false, false,
                _("Initial Statistics window width") );

  all_options["Statistics window ysize"] = (Option*) new 
    LongOption( GROUP_SIZE_OF_WINDOWS, 428, 130, 10000, false, false,
                _("Initial Statistics window height") );

  gtk_opts = new OptionHandler( "Gui Gtk", all_options, group_names );
}

int
Gui::main_loop( GuiClientInterface* _gi_p )
{
  guiinterface_p = _gi_p;
  set_colours();

  controlwindow_p = 
    new ControlWindow( -1, -1,
                       gtk_opts->get_l( OPTION_CONTROL_WINDOW_POS_X ),
                       gtk_opts->get_l( OPTION_CONTROL_WINDOW_POS_Y ) );


  gint timeout_tag;      
//    double interval = 1000.0*the_opts.get_d( OPTION_UPDATE_INTERVAL ) - 10.0; 
  double interval = 1000.0*0.05 - 10.0; 
  timeout_tag = gtk_timeout_add( (unsigned int) interval,
                                 GtkFunction(Gui::update_function), (gpointer) NULL );

  gtk_main();

  return EXIT_SUCCESS;
}

int
Gui::timeout_function()
{
  get_information();
  return true;
}

void
Gui::get_information()
{
  const InfoBase* info_p;
  while( NULL != ( info_p = guiinterface_p->check_information() ) )
    {
      game_time = info_p->get_game_time();
      switch( info_p->get_type() )
        {
        case INFO_UNKNOWN:
          break;
        case INFO_QUIT:
          {
            gtk_main_quit();
            GIExit(EXIT_SUCCESS);
          }
          break;
        case INFO_STATE:
          {
            state = ((StateInfo*)info_p)->get_state();
            the_controlwindow.set_window_title( state );
            break;
          }
        case INFO_TOURNAMENT_STARTED:
          break;
        case INFO_TOURNAMENT_ENDED:
          break;
        case INFO_GAME_STARTED:
          {
            GameStartedInfo* gameinfo_p = (GameStartedInfo*)info_p;
            match_nr = gameinfo_p->get_match();
            game_nr  = gameinfo_p->get_game();
            // TODO: Take care of participating_robots
            clear_and_delete_pointer_list( drawing_objects_lists[WALL] );
            int id = -1;
            list<object_pos_info*>::const_iterator opi;
            for( opi = gameinfo_p->get_list_of_walls().begin();
                 opi != gameinfo_p->get_list_of_walls().end(); opi++ )
              {
                id++;
                DrawingShape* ds_p = NULL;
                switch( (*opi)->type )
                  {
                  case SHAPE_LINE:
                    ds_p = new DrawingLine( id, fg_rgb_colour );
                    break;
                  case SHAPE_CIRCLE:
                    ds_p = new DrawingCircle( id, fg_rgb_colour );
                    break;
                  case SHAPE_INNER_CIRCLE:
                    ds_p = new DrawingInnerCircle( id, fg_rgb_colour );
                    break;
                  case SHAPE_ARC:
                    ds_p = new DrawingArc( id, fg_rgb_colour );
                    break;
                  }
                ds_p->set_position( *opi );
              }
          break;
          }
        case INFO_MESSAGE:
          if( is_messagewindow_up() )
            {
              messagewindow_p->freeze_clist();
              const list<MessageInfo::message> message_list
                = ((MessageInfo*)info_p)->get_message_list();
              list<MessageInfo::message>::const_iterator mci;
              for( mci = message_list.begin(); mci != message_list.end(); mci++ )
                messagewindow_p->add_message( (*mci).sender, (*mci).message );
              messagewindow_p->thaw_clist();
            }
          break;
        }
    }
}

void
Gui::set_colours()
{  
//    bg_rgb_colour = the_opts.get_l( OPTION_BACKGROUND_COLOUR );
//    fg_rgb_colour = the_opts.get_l( OPTION_FOREGROUND_COLOUR );
//    rtb_message_rgb_colour = the_opts.get_l( OPTION_RTB_MESSAGE_COLOUR );

  bg_rgb_colour = 16445670;
  fg_rgb_colour = 0;
  rtb_message_rgb_colour = 1118702;

  bg_gdk_colour = make_gdk_colour( bg_rgb_colour );
  fg_gdk_colour = make_gdk_colour( fg_rgb_colour );
  rtb_message_gdk_colour = make_gdk_colour( rtb_message_rgb_colour );
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
Gui::open_optionswindow()
{
  if( NULL == optionswindow_p )
    optionswindow_p = new OptionsWindow( -1, -1, -1, -1 );
}

void
Gui::close_optionswindow()
{
  if( NULL != optionswindow_p )
    {
      delete optionswindow_p;
      optionswindow_p = NULL;
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
//    if( the_arena_controller.is_started() )
//      if( NULL == statisticswindow_p && the_arena.get_state() != NOT_STARTED )
//        statisticswindow_p = 
//          new StatisticsWindow( the_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_X ),
//                                the_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_Y ),
//                                -1, -1 );
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
//    bool open = false;
//    if( NULL == starttournamentwindow_p )
//      {
//        if( the_arena_controller.is_started() )
//          if( the_arena.get_state() != NOT_STARTED &&
//              the_arena.get_state() != FINISHED )
//            {
//              string info_text = _("This action will kill the current tournament.\n"
//                                   "Do you want to do that?");
//              list<string> string_list;
//              string_list.push_back( string( _("Yes") ) );
//              string_list.push_back( string( _("No")  ) );
//              Dialog( info_text, string_list,
//                      (DialogFunction) Gui::kill_and_start_new_tournament );
//            }
//          else
//            open = true;
//        else
//          open = true;
//      }

//    if( open )
//      starttournamentwindow_p = 
//        new StartTournamentWindow( -1, -1, -1, -1 );
}

void
Gui::kill_and_start_new_tournament( int result )
{
//    if( the_arena_controller.is_started() && result == 1 )
//      {
//        the_arena.interrupt_tournament();
//        the_gui.open_starttournamentwindow();
//      }
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

gint
Gui::update_function(gpointer data)
{  
  gint res = 1;

  res = (gint)the_gui.timeout_function();

  return res;
}
