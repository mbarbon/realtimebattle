/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <string>
#include <iostream>
#include <strstream>

#include "ArenaWindow.h"
#include "ControlWindow.h"
#include "Dialog.h"
#include "DrawingObjects.h"
#include "Gui.h"
#include "GuiVarious.h"
#include "MessageWindow.h"
#include "OptionsWindow.h"
#include "ScoreWindow.h"
#include "StatisticsWindow.h"
#include "StartTournamentWindow.h"

#include "GuiFunctions.h"
#include "GuiInterface.h"
#include "InfoClasses.h"
#include "IntlDefs.h"
#include "Messagetypes.h"
#include "Option.h"
#include "OptionHandler.h"
#include "Structs.h"
#include "Various.h"

int gui_returncode;
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

  gui_returncode = EXIT_SUCCESS;

  gui_p = new Gui();
  return (gui_p != NULL);
}

int
GIMain( GuiClientInterface*  _gi_p )
{
  gui_p->main_loop( _gi_p );

  cout<<"End of the Gui main loop\n";

  _gi_p->quit_program( true );
  delete gui_p;
  return gui_returncode;
}

int
GICommand( string command )
{
}

int
GIhandle_agreement_packet( TournamentCommitChangePacket* p)
{
  gui_p->get_starttournament_p()->handle_packet( p );
  return 0;
}

Gui::Gui()
{
  main_opts = NULL;
  statisticswindow_p = NULL;
  starttournamentwindow_p = NULL;

  gui_interface_p = NULL;

  debug_level = 0;
  state = NO_STATE;

  initialize_gtk_options();
}

Gui::~Gui()
{
  close_statisticswindow();
  close_starttournamentwindow();
  close_scorewindow();
  close_optionswindow();
  close_messagewindow();
  close_arenawindow();

  delete controlwindow_p;

  for( int i = ROBOT; i < LAST_OBJECT_TYPE; i++ )
    clear_and_delete_pointer_list( drawing_objects_lists[i] );
}

void
Gui::initialize_gtk_options()
{
  // Note: Must begin at 0 and be consecutive.
  const int GROUP_SIZE_OF_WINDOWS = 0;
  const int GROUP_MISC            = 1;
  const int GROUP_LAST_GROUP      = 2;

  map<string,Option*> all_options;
  vector<string> group_names( GROUP_LAST_GROUP, "" );

  group_names[GROUP_SIZE_OF_WINDOWS] =  _("Window sizes");
  group_names[GROUP_MISC]            =  _("Miscellaneous");


  // ---------- Size of Windows ----------

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

  // ---------- Misc ----------

  all_options["Message history length"] = (Option*) new
    LongOption( GROUP_MISC, 500, -1, 1000000, false, false,
                _("History length for robot messages") );

  all_options["RTB message colour"] = (Option*) new
    LongOption( GROUP_MISC, 0x1111ee, 0x000000, 0xffffff, false, false,
                _("Colour for RTB messages"), true );

  gtk_opts = new OptionHandler( "Gui Gtk", all_options, group_names );
}

int
Gui::main_loop( GuiClientInterface* _gi_p )
{

  gui_interface_p = (GuiClientInterface*) _gi_p;

  set_colours();

  controlwindow_p = 
    new ControlWindow( -1, -1, 
                       gtk_opts->get_l( "Control window xpos" ),
                       gtk_opts->get_l( "Control window ypos" ) );

  gint timeout_tag;      
//    double interval = 1000.0*the_gtk_opts.get_d( OPTION_UPDATE_INTERVAL ) - 10.0; 
  double interval = 1000.0*0.05 - 10.0;

  timeout_tag = gtk_timeout_add( (unsigned int) interval,
                                 GtkFunction(Gui::update_function), (gpointer) NULL );

  gtk_main();

  return gui_returncode;
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
  while( NULL != ( info_p = gui_interface_p->check_information() ) )
    {
      game_time = info_p->get_game_time();
      switch( info_p->get_type() )
        {
        case INFO_UNKNOWN:
          break;
        case INFO_QUIT:
          quit( false );
          break;
        case INFO_STATE:
          {
            state = ((StateInfo*)info_p)->get_state();
            the_controlwindow.set_status( state );
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
          {
            const list<message_t>& message_list
              = ((MessageInfo*)info_p)->get_message_list();
            messagewindow.add_messages( message_list );
          }
          break;
        case INFO_OPTION_DEFINITION:
          {
            if( main_opts != NULL )
              break;

            const OptionDefinitionInfo* od_info_p =
              (OptionDefinitionInfo*)info_p;
            string section_name = od_info_p->get_section_name();
            vector< string > group_names = od_info_p->get_group_names();
            map< string, Option* > all_options_copy;

            map< string, Option* >::const_iterator soci;
            for( soci  = od_info_p->get_all_options().begin();
                 soci != od_info_p->get_all_options().end();
                 soci++ )
              {
                all_options_copy[ soci->first ] = soci->second;
              }
            main_opts = new OptionHandler( section_name,
                                           all_options_copy,
                                           group_names );
          }
          break;
        }
    }
}

void
Gui::apply_request( GuiRequest* req )
{
  gui_interface_p->apply_request( req );
}

void
Gui::set_colours()
{  
//    bg_rgb_colour = the_gtk_opts.get_l( OPTION_BACKGROUND_COLOUR );
//    fg_rgb_colour = the_gtk_opts.get_l( OPTION_FOREGROUND_COLOUR );

  bg_rgb_colour = 16445670;
  fg_rgb_colour = 0;

  bg_gdk_colour = make_gdk_colour( bg_rgb_colour );
  fg_gdk_colour = make_gdk_colour( fg_rgb_colour );
}

// TODO: Cleanly destruct everything in the gui
void
Gui::quit( bool exit_program )
{
  if( exit_program )
    gui_interface_p->quit_program( EXIT_SUCCESS );
  gui_returncode = EXIT_SUCCESS;
  gtk_main_quit();
}

// TODO: Cleanly destruct everything in the gui
void
Gui::error( const bool fatal, const string& error_msg, const string& function_name )
{
  cerr << "RealTimeBattle Gtk gui: " << _("Error in") << " "
       << function_name << ": " << error_msg << endl;
  if( fatal )
    {
      gui_interface_p->quit_program( EXIT_FAILURE );
      gui_returncode = EXIT_FAILURE;
      gtk_main_quit();
    }
}

void
Gui::open_arenawindow()
{
  arenawindow.create( gtk_opts->get_l( "Arena window xsize" ),
                      gtk_opts->get_l( "Arena window ysize" ),
                      gtk_opts->get_l( "Arena window xpos" ),
                      gtk_opts->get_l( "Arena window ypos" ) );
}

void
Gui::close_arenawindow()
{
  arenawindow.destroy();
}

void
Gui::open_messagewindow()
{
  messagewindow.create( gtk_opts->get_l( "Message window xsize" ),
                        gtk_opts->get_l( "Message window ysize" ),
                        gtk_opts->get_l( "Message window xpos" ),
                        gtk_opts->get_l( "Message window ypos" ) );
}

void
Gui::close_messagewindow()
{
  messagewindow.destroy();;
}

void
Gui::open_optionswindow()
{
  optionswindow.create( -1, -1, -1, -1 );
}

void
Gui::close_optionswindow()
{
  optionswindow.destroy();
}

void
Gui::open_scorewindow()
{
  scorewindow.create( gtk_opts->get_l( "Score window xsize" ),
                      gtk_opts->get_l( "Score window ysize" ),
                      gtk_opts->get_l( "Score window xpos" ),
                      gtk_opts->get_l( "Score window ypos" ) );
}

void
Gui::close_scorewindow()
{
  scorewindow.destroy();
}

void
Gui::open_statisticswindow()
{
//    if( the_arena_controller.is_started() )
//      if( NULL == statisticswindow_p && the_arena.get_state() != NOT_STARTED )
//        statisticswindow_p = 
//          new StatisticsWindow( the_gtk_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_X ),
//                                the_gtk_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_Y ),
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
Gui::open_starttournamentwindow(bool create_channel)
{
    bool open = false;
    if( NULL == starttournamentwindow_p )
      {
        /*if( the_arena_controller.is_started() )
          if( the_arena.get_state() != NOT_STARTED &&
              the_arena.get_state() != FINISHED )
            {
              string info_text = _("This action will kill the current tournament.\n"
                                   "Do you want to do that?");
              list<string> string_list;
              string_list.push_back( string( _("Yes") ) );
              string_list.push_back( string( _("No")  ) );
              Dialog( info_text, string_list,
                      (DialogFunction) Gui::kill_and_start_new_tournament );
            }
          else
            open = true;
	    else */
	open = true;
      }
    
    if( open ) 
      {
	gui_p->apply_request(new OpenTournamentAgreementChannelRequest(create_channel));
	starttournamentwindow_p = 
	  new StartTournamentWindow( -1, -1, -1, -1 );
      }
}

void
Gui::kill_and_start_new_tournament( int result )
{
//    if( the_arena_controller.is_started() && result == 1 )
//      {
//        the_arena.interrupt_tournament();
//        gui_p->open_starttournamentwindow();
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

  res = (gint)gui_p->timeout_function();

  return res;
}

