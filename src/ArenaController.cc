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
#include <config.h>
#endif

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#else
# include "getopt.h"
#endif
#include <stdlib.h>

#include <pthread.h>
#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <math.h>

#include "ArenaController.h"
#include "Arena.h"
#include "OptionHandler.h"
#include "IntlDefs.h"
#include "GuiInterface.h"
#include "Structs.h"
#include "String.h"
#include "Various.h"

// ArenaController constructor

ArenaController::ArenaController()
{
  //  started = false;

  option_filename = "";
  statistics_filename = "";
  log_filename = "";
  tournament_filename = "";
  message_filename = "";
  replay_filename = "";

  game_mode = NORMAL_MODE;
  debug_level = 1;
  auto_start_and_end = false;

  pthread_mutex_init( &gi_mutex, NULL );
}

ArenaController::~ArenaController()
{
  //  if( started ) close_arena();

  list<GuiServerInterface*>::iterator li;
  for( li = gui_list.begin(); li != gui_list.end(); li++ )
    {
      (*li)->shutdown();
      delete ((GuiInterface*)*li);
    }
  gui_list.clear();

  pthread_mutex_destroy( &gi_mutex );

  delete main_opts;
}

int
ArenaController::init( int argc, char** argv )
{
  initialize_options();

  parse_command_line( argc, argv );

  // Startup all guis
  list<GuiServerInterface*>::iterator li;
  for( li = gui_list.begin(); li != gui_list.end(); li++ )
    (*li)->startup();

  if( tournament_filename != "" )
    ; // Create a StartNewTournamentEvent

  else if( replay_filename != "" )
      ;    //  Create a StartReplayEvent

  return EXIT_SUCCESS; // TODO: what should be returned here?
}

void
ArenaController::initialize_options()
{
  map<string,Option*> all_options;
  vector<string> group_names;

  group_names.push_back( _("Environment") );
  group_names.push_back( _("Robot") );
  group_names.push_back( _("Shot") );
  group_names.push_back( _("Extras") );
  group_names.push_back( _("Time") );
//    group_names.push_back( _("Window sizes") );
  group_names.push_back( _("Misc") );

  const int GROUP_ENVIRONMENT = 0;
  const int GROUP_ROBOT = 1;
  const int GROUP_SHOT = 2;
  const int GROUP_EXTRAS = 3;
  const int GROUP_TIME = 4;
  const int GROUP_MISC = 5;

//    DoubleOption* all_double_options =
//      new DoubleOption[LAST_DOUBLE_OPTION];
//    LongOption* all_long_options =
//      new LongOption[LAST_LONG_OPTION];
//    StringOption* all_string_options =
//      new StringOption[LAST_STRING_OPTION];

  // ---------- Environment ----------

  all_options["Gravity"] = (Option*) new 
    DoubleOption( GROUP_ENVIRONMENT, 9.82, 0.2, 20.0, false, true,
                  _("Gravitational Constant") );

//    all_double_options[OPTION_AIR_RESISTANCE] = 
//      DoubleOption( GROUP_ENVIRONMENT, 0.005, 0.0, 1.0, false, true,
//                    "Air resistance", _("Air resistance") );

//    all_double_options[OPTION_ROLL_FRICTION] = 
//      DoubleOption( GROUP_ENVIRONMENT, 0.002, 0.0, 1.0, false, true,
//                    "Roll friction", _("Roll friction") );

//    all_double_options[OPTION_SLIDE_FRICTION] = 
//      DoubleOption( GROUP_ENVIRONMENT, 0.1, 0.0, 5.0, false, true,
//                    "Slide/brake friction", _("Slide/brake friction") );

//    // ---------- Robot ----------

//    all_double_options[OPTION_ROBOT_MAX_ACCELERATION] = 
//      DoubleOption( GROUP_ROBOT, 2.0, 0.1, 10.0, true, true,
//                    "Robot max acceleration", _("Robot max acceleration") );

//    all_double_options[OPTION_ROBOT_MIN_ACCELERATION] = 
//      DoubleOption( GROUP_ROBOT, -0.5, -10.0, 0.0, true, true,
//                    "Robot min acceleration", _("Robot min acceleration") );
    
//    all_double_options[OPTION_ROBOT_RADIUS] = 
//      DoubleOption( GROUP_ROBOT, 0.5, 0.1, 10.0, false, true,
//                    "Robot radius", _("Robot radius") );

//    all_double_options[OPTION_ROBOT_MASS] = 
//      DoubleOption( GROUP_ROBOT, 1.0, 0.01, 100.0, false, true,
//                    "Robot mass", _("Robot mass") );

//    all_double_options[OPTION_ROBOT_BOUNCE_COEFF] = 
//      DoubleOption( GROUP_ROBOT, 0.7, 0.0, 1.0, false, true,
//                    "Robot bounce coefficient", _("Robot bounce coefficient") );

//    all_double_options[OPTION_ROBOT_HARDNESS] = 
//      DoubleOption( GROUP_ROBOT, 0.5, 0.0, 1.0, false, true,
//                    "Robot hardness coefficient", _("Robot hardness coefficient") );

//    all_double_options[OPTION_ROBOT_PROTECTION] = 
//      DoubleOption( GROUP_ROBOT, 0.5, 0.0, 1.0, false, true,
//                    "Robot protection coefficient", _("Robot protection coefficient") );

//    all_double_options[OPTION_ROBOT_FRONTSIZE] = 
//      DoubleOption( GROUP_ROBOT, M_PI/3.0, 0.0, M_PI*2.0, false, true,
//                    "Robot frontsize [radians]", _("Robot frontsize [radians]") );

//    all_double_options[OPTION_ROBOT_FRONT_BOUNCE_COEFF] = 
//      DoubleOption( GROUP_ROBOT, 0.7, 0.0, 1.0, false, true,
//                    "Robot front bounce coefficient",
//                    _("Robot front bounce coefficient") );

//    all_double_options[OPTION_ROBOT_FRONT_HARDNESS] = 
//      DoubleOption( GROUP_ROBOT, 0.9, 0.0, 1.0, false, true,
//                    "Robot front hardness coefficient",
//                    _("Robot front hardness coefficient") );

//    all_double_options[OPTION_ROBOT_FRONT_PROTECTION] = 
//      DoubleOption( GROUP_ROBOT, 0.9, 0.0, 1.0, false, true,
//                    "Robot front protection coefficient",
//                    _("Robot front protection coefficient") );

//    all_double_options[OPTION_ROBOT_START_ENERGY] = 
//      DoubleOption( GROUP_ROBOT, 100.0, 0.01, 10000.0, true, true,
//                    "Robot start energy", _("Robot start energy") );

//    all_double_options[OPTION_ROBOT_MAX_ENERGY] = 
//      DoubleOption( GROUP_ROBOT, 120.0, 0.01, 10000.0, true, true,
//                    "Robot max energy", _("Robot max energy") );

//    all_double_options[OPTION_ROBOT_MAX_ROTATE] = 
//      DoubleOption( GROUP_ROBOT, 2.0*M_PI / 8.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0,
//                            true, true, "Robot max rotate speed [rad/s]",
//                            _("Robot max rotate speed [rad/s]") );

//    all_double_options[OPTION_ROBOT_CANNON_MAX_ROTATE] = 
//      DoubleOption( GROUP_ROBOT, 2.0*M_PI / 4.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0,
//                            true, true, "Robot cannon max rotate speed [rad/s]",
//                            _("Robot cannon max rotate speed [rad/s]") );

//    all_double_options[OPTION_ROBOT_RADAR_MAX_ROTATE] = 
//      DoubleOption( GROUP_ROBOT, 2.0*M_PI / 3.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0,
//                            true, true, "Robot radar max rotate speed [rad/s]",
//                            _("Robot radar max rotate speed [rad/s]") );

//    all_long_options[OPTION_ROBOT_ENERGY_LEVELS] = 
//      LongOption( GROUP_ROBOT, 10, 1, 100, true, true,
//                  "Robot energy levels", _("Robot energy levels") );

//    // ---------- Shot ----------

//    all_double_options[OPTION_SHOT_RADIUS] = 
//      DoubleOption( GROUP_SHOT, 0.1, 0.01, 1.0, false, true,
//                    "Shot radius", _("Shot radius") );

//    all_double_options[OPTION_SHOT_SPEED] = 
//      DoubleOption( GROUP_SHOT, 10.0, 0.1, 100.0, true, true,
//                    "Shot speed", _("Shot speed") );

//    all_double_options[OPTION_SHOOTING_PENALTY] = 
//      DoubleOption( GROUP_SHOT, 0.075, 0.0, 1.0, false, true,
//                    "Shooting penalty (times shot energy)",
//                    _("Shooting penalty (times shot energy)") );

//    all_double_options[OPTION_SHOT_MIN_ENERGY] = 
//      DoubleOption( GROUP_SHOT, 0.5, 0.0, 100.0, true, true,
//                    "Shot min energy", _("Shot min energy") );

//    all_double_options[OPTION_SHOT_MAX_ENERGY] = 
//      DoubleOption( GROUP_SHOT, 30.0, 0.0, 100000000.0, true, true,
//                    "Shot max energy", _("Shot max energy") );

//    all_double_options[OPTION_SHOT_ENERGY_INCREASE_SPEED] = 
//      DoubleOption( GROUP_SHOT, 10.0, 0.0, 100000000.0, true, true,
//                    "Shot energy increase speed [energy/s]",
//                    _("Shot energy increase speed [energy/s]") );

//    // ---------- Extras ----------

//    all_double_options[OPTION_COOKIE_MAX_ENERGY] = 
//      DoubleOption( GROUP_EXTRAS, 15.0, 0.0, 100000000.0, false, true,
//                    "Cookie max energy", _("Cookie max energy") );

//    all_double_options[OPTION_COOKIE_MIN_ENERGY] = 
//      DoubleOption( GROUP_EXTRAS, 10.0, 0.0, 100000000.0, false, true,
//                    "Cookie min energy", _("Cookie min energy") );

//    all_double_options[OPTION_COOKIE_FREQUENCY] = 
//      DoubleOption( GROUP_EXTRAS, 0.03, 0.0, 100000000.0, false, true,
//                    "Cookie frequency [cookies per second]",
//                    _("Cookie frequency [cookies per second]") );

//    all_double_options[OPTION_COOKIE_RADIUS] = 
//      DoubleOption( GROUP_EXTRAS, 0.3, 0.01, 1.0, false, true,
//                    "Cookie radius", _("Cookie radius") );

//    all_long_options[OPTION_COOKIE_COLOUR] = 
//      LongOption( GROUP_EXTRAS, 0x35d715, 0x000000, 0xffffff, false, false,
//                  "Cookie colour", _("Cookie colour"), true );

//    all_double_options[OPTION_MINE_MAX_ENERGY] = 
//      DoubleOption( GROUP_EXTRAS, 25.0, 0.0, 100000000.0, false, true,
//                    "Mine max energy", _("Mine max energy") );

//    all_double_options[OPTION_MINE_MIN_ENERGY] = 
//      DoubleOption( GROUP_EXTRAS, 15.0, 0.0, 100000000.0, false, true,
//                    "Mine min energy", _("Mine min energy") );

//    all_double_options[OPTION_MINE_FREQUENCY] = 
//      DoubleOption( GROUP_EXTRAS, 0.06, 0.0, 100000000.0, false, true,
//                    "Mine frequency [mines per second]",
//                    _("Mine frequency [mines per second]") );

//    all_double_options[OPTION_MINE_RADIUS] = 
//      DoubleOption( GROUP_EXTRAS, 0.3, 0.01, 1.0, false, true,
//                    "Mine radius", _("Mine radius") );

//    all_long_options[OPTION_MINE_COLOUR] = 
//      LongOption( GROUP_EXTRAS, 0xff0000, 0x000000, 0xffffff, false, false,
//                  "Mine colour", _("Mine colour"), true );

//    // ---------- Time ----------

//    all_double_options[OPTION_CHECK_INTERVAL] = 
//      DoubleOption( GROUP_TIME, 1.0, 0.01, 1000000, false, false,
//                    "Process check interval", _("Process check interval") );
    
//    all_double_options[OPTION_TIMEOUT] = 
//      DoubleOption( GROUP_TIME, 120.0, 1.0, 100000000.0, false, true,
//                    "Timeout [s]", _("Timeout [s]") );

//    all_double_options[OPTION_MAX_TIMESTEP] = 
//      DoubleOption( GROUP_TIME, 0.1, 0.001, 1.0, false, false,
//                    "Max timestep", _("Max timestep") );

//    all_double_options[OPTION_TIMESCALE] = 
//      DoubleOption( GROUP_TIME, 1.0, 0.01, 100.0, false, false,
//                    "Timescale", _("Timescale") );

//    all_double_options[OPTION_UPDATE_INTERVAL] = 
//      DoubleOption( GROUP_TIME, 0.050, 0.010, 1.0, false, false,
//                    "Update interval [s]", _("Update interval [s]") );

//    all_double_options[OPTION_ROBOT_STARTUP_TIME] = 
//      DoubleOption( GROUP_TIME, 1.0, 0.050, 1000000.0, false, false,
//                    "Robot startup time [s]", _("Robot startup time [s]") );

//    all_double_options[OPTION_CPU_START_LIMIT] = 
//      DoubleOption( GROUP_TIME, 5.0, 0.01, 100000000.0, false, false,
//                    "Start CPU time [s]", _("Start CPU time [s]") );

//    all_double_options[OPTION_CPU_EXTRA] = 
//      DoubleOption( GROUP_TIME, 2.5, 0.01, 100000000.0, false, false,
//                    "Extra CPU time [s]", _("Extra CPU time [s]") );

//    all_double_options[OPTION_CPU_PERIOD] = 
//      DoubleOption( GROUP_TIME, 60.0, 1.0, 100000000.0, false, false,
//                    "Extra CPU period [s]", _("Extra CPU period [s]") );

//    all_double_options[OPTION_CPU_WARNING_PERCENT] = 
//      DoubleOption( GROUP_TIME, 0.9, 0.1, 1.0, false, false,
//                    "CPU time warning percentage", _("CPU time warning percentage") );

//    all_long_options[OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL] =
//      LongOption( GROUP_TIME, 1, 1, 1000, false, false,
//                  "Logging frequency [Each nth update interval]",
//                  _("Logging frequency [Each n:th update interval]") );

  // ---------- Misc ----------

  all_options["Background colour"] = (Option*) new
    LongOption( GROUP_MISC, 0xfaf0e6, 0x000000, 0xffffff, false, false,
                _("Background colour"), true );

  all_options["Foreground colour"] = (Option*) new
    LongOption( GROUP_MISC, 0x000000, 0x000000, 0xffffff, false, false,
                _("Foreground colour"), true );

  all_options["RTB message colour"] = (Option*) new
    LongOption( GROUP_MISC, 0x1111ee, 0x000000, 0xffffff, false, false,
                _("Colour for RTB messages"), true );


//    all_double_options[OPTION_ARENA_SCALE] = 
//      DoubleOption( GROUP_MISC, 1.0, 0.001, 1000, false, true,
//                    "Arena scale", _("Arena scale") );

//    all_double_options[OPTION_FAST_FORWARD_FACTOR] = 
//      DoubleOption( GROUP_MISC, 5.0, 0.001, 1000, false, false,
//                    "Fast forward factor", _("Fast forward factor") );

//    all_long_options[OPTION_MAX_ROBOTS_ALLOWED] =
//      LongOption( GROUP_MISC, 120, 1, 10000, false, false,
//                  "Max robots allowed simultaneously",
//                  _("Max robots allowed simultaneously") );

//    all_string_options[OPTION_ROBOT_SEARCH_PATH] =
//      StringOption( GROUP_MISC, "", 1000, false, false,
//                    "Robot search path", _("Robot search path") );

//    all_string_options[OPTION_ARENA_SEARCH_PATH] =
//      StringOption( GROUP_MISC, "", 1000, false, false,
//                    "Arena search path", _("Arena search path") );

//    all_string_options[OPTION_TMP_RTB_DIR] =
//      StringOption( GROUP_MISC, "/tmp/rtb", 1000, false, false,
//                    "Directory for temporary files", _("Directory for temporary files") );


  main_opts = new OptionHandler( "RealTimeBattle", all_options, group_names );
//    main_opts = new OptionHandler( "RealTimeBattle",
//                                   all_double_options, all_long_options,
//                                   all_string_options,
//                                   LAST_DOUBLE_OPTION, LAST_LONG_OPTION,
//                                   LAST_STRING_OPTION );
}

void
ArenaController::quit_gui( GuiServerInterface* gui_p, bool exit_program )
{
  list<GuiServerInterface*>::iterator li;
  if( exit_program )
    {
      for( li = gui_list.begin(); li != gui_list.end(); li++ )
        {
          (*li)->shutdown();
          //TODO: Find a way to delete *li in a clean manner
          //          delete ((GuiInterface*)*li);
        }
      gui_list.clear();
      Quit();
    }
  else
    {
      if( (li = find( gui_list.begin(), gui_list.end(), gui_p )) != gui_list.end() )
        {
          (*li)->shutdown();
          //TODO: Find a way to delete *li in a clean manner
          //          delete ((GuiInterface*)*li);
          gui_list.erase(li);
        }
    }

}

//  void
//  ArenaController::start_arena()
//  {
//    if( started ) close_arena();

//    my_arena = new Arena();
  
//    started = true;
//  }

//  void
//  ArenaController::close_arena()
//  {
//    if( started )
//      {
//        delete my_arena;
//      }

//    started = false;
//  }

void
ArenaController::parse_command_line( int argc, char** argv )
{
  int version_flag=false, help_flag=false;
  int c;

  extern char* optarg;
  extern int optind;

  static struct option long_options[] =
  {
    //option, argument?, flag, value
    {"version", 0, &version_flag, true},
    {"help", 0, &help_flag, true},

    {"debug_mode", 0, &game_mode, DEBUG_MODE},
    {"debug_level", 1, 0, 0},
    {"normal_mode", 0, &game_mode, NORMAL_MODE},
    {"competition_mode", 0, &game_mode, COMPETITION_MODE},
    {"option_file", 1, 0, 0},
    {"log_file", 1, 0, 0},
    {"statistics_file", 1, 0, 0},
    {"tournament_file", 1, 0, 0},
    {"message_file", 1, 0, 0},
    {"replay", 1, 0, 0},
    {"use_gui", 1, 0, 0},

    {0, 0, 0, 0}
  };

  for(;;)
    {
      int option_index = 0;
     
      c = getopt_long( argc, argv, "dncD:vho:l:s:t:m:r:g:",
                       long_options, &option_index );

      // Detect the end of the options.
      if (c == -1)
        break;
     
      switch (c)
        {

        case 0:
          // If this option set a flag, do nothing else now.
          if( long_options[option_index].flag != 0 )
            break;
          
          switch( option_index )
            {
            case 3:
              debug_level = chars2int( optarg );
              game_mode = DEBUG_MODE;
              break;
            case 6: 
              option_filename = (string)optarg;
              break;
            case 7:
              log_filename = (string)optarg;
              break;
            case 8:
              statistics_filename = (string)optarg;
              break;
            case 9:
              tournament_filename = (string)optarg;
              auto_start_and_end = true;
              break;
            case 10:
              message_filename = (string)optarg;
              break;
            case 11:
              replay_filename = (string)optarg;
              break;
            case 12:
              gui_list.push_back( new GuiInterface( optarg, &gi_mutex, argc, argv ) );
              break;
            default:
              Error( true, "Bad error: Nonexisting options. This shouldn't happen",
                     "ArenaController.cc::parse_command_line" );
              exit( EXIT_FAILURE );
            }
          break;


        case 'd':
          game_mode = DEBUG_MODE;
          break;

        case 'D':
          debug_level = chars2int( optarg );
          game_mode = DEBUG_MODE;
          break;

        case 'n':
          game_mode = NORMAL_MODE;
          break;

        case 'c':
          game_mode = COMPETITION_MODE;
          break;

        case 'v':
          version_flag = true;
          break;

        case 'h':
          help_flag = true;
          break;

        case 'o':
          option_filename = (string)optarg;
          break;

        case 'l':
          log_filename = (string)optarg;
          break;

        case 's':
          statistics_filename = (string)optarg;
          break;

        case 't':
          tournament_filename = (string)optarg;
          auto_start_and_end = true;
          break;

        case 'm':
          message_filename = (string)optarg;
          break;

        case 'r':
          replay_filename = (string)optarg;
          break;

        case 'g':
          gui_list.push_back( ((GuiServerInterface*)
                               new GuiInterface( optarg, &gi_mutex, argc, argv )) );
          break;

        default:
          print_help_message();
          exit( EXIT_FAILURE );
        }
    }

  if( debug_level > max_debug_level )
    debug_level = 5;
  if( debug_level < 0 )
    debug_level = 0;

  // TODO: A better way to determine whether a tournament can be stated
  //       (i.e. some guis may not start up a tournament)
  if( optind != argc ||
      ( tournament_filename == "" && gui_list.empty() ) )
    {
      print_help_message();
      exit( EXIT_FAILURE );
    }

  if( version_flag )
      cout << "RealTimeBattle " << VERSION << endl;

  if( help_flag )
    print_help_message();

  if( help_flag || version_flag ) exit( EXIT_SUCCESS );

  if( option_filename == "" )
    {
      the_opts.read_options_from_rtbrc();
      option_filename = ".rtbrc";
    }
  else
    the_opts.read_options_file( option_filename,true );

  auto_start_and_end
    = ( ( tournament_filename != "" ) ||
        ( replay_filename == "-" ) );
}


void 
ArenaController::print_help_message()
{
  cout << endl;
  cout << _(" Usage: RealTimeBattle [options] ") << endl << endl;
  cout << _(" Options:") << endl;
  cout << _("    --debug_mode,                -d   debug mode") << endl;
  cout << _("    --debug_level [0-5],         -D   sets the initial debug level. Implies -d") << endl;
  cout << _("    --normal_mode,               -n   normal mode (default)") << endl;
  cout << _("    --competition_mode,          -c   competition mode") << endl ;
  cout << endl;
  cout << _("    --use_gui [gui],             -g   sets the gui to be used") << endl ;
  cout << _("    --option_file [file],        -o   selects option-file (default: $HOME/.rtbrc)")  << endl;
  cout << endl;
  cout << _("    --log_file [file],           -l   make log file, if 'file' is '-'\n"
            "                                      the log is sent to STDOUT") << endl;
  cout << endl;
  cout << _("    --tournament_file [file],    -t   specify a tournament file to\n"
            "                                      autostart a tournament") << endl;
  cout << endl;
  cout << _("    --statistics_file [file],    -s   file to print the statistics to\n"
            "                                       when autostarting") << endl;
  cout << endl;
  cout << _("    --message_file [file],       -m   redirect messages to 'file'.\n"
            "                                      With '-' as 'file', STDOUT is used.\n"
            "                                      If both log and messages are sent\n"
            "                                      to STDOUT, '-m' will be ignored") << endl;
  cout << endl;
  cout << _("    --replay [file]              -r   a log file to replay.\n"
            "                                      if '-' is specified as file,\n"
            "                                      input is taken from STDIN") << endl;
  cout << endl;
  cout << _("    --help,                      -h   prints this message") << endl;
  cout << _("    --version,                   -v   prints the version number") << endl;
  cout << endl;

  list<GuiServerInterface*>::const_iterator li;
  for( li = gui_list.begin(); li != gui_list.end(); li++ )
    {
      cout << endl;
      cout << " " << _("Commandline options for gui") << " '" << (*li)->Name() << "':" << endl;
      cout << (*li)->UsageMessage() << endl;
    }
}
