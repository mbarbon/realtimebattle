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

#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream.h>
#include <iomanip.h>
#include <stdarg.h>

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include "Gui.h"
//#include "Arena.h"
//#include "MovingObject.h"
//#include "Shape.h"
#include "Extras.h"
#include "Various.h"
#include "String.h"

Arena::Arena()
{
  state = NOT_STARTED;
  game_mode = NORMAL_MODE;

  halted = false;
  halt_next = false;
  paus_after_next_game = false;
  timer = g_timer_new();
  reset_timer();
  
  all_robots_in_sequence = g_list_alloc();
  all_robots_in_tournament = g_list_alloc();
  arena_filenames = g_list_alloc();
  exclusion_points = g_list_alloc();

  for(int i=ROBOT; i<=EXPLOSION; i++)
    object_lists[i] = g_list_alloc();

  max_debug_level = 5;
  debug_level = 0;
}

Arena::~Arena()
{
  //  if( filep ) delete filep;
  state=EXITING;
  sleep(1);
  g_timer_destroy(timer);
  delete_lists(true, true, true, true);
  g_list_free(all_robots_in_sequence);
  g_list_free(all_robots_in_tournament);
  g_list_free(arena_filenames);
  g_list_free(exclusion_points);
  for(int i=ROBOT; i<EXPLOSION; i++)
    g_list_free(object_lists[i]);

  for(int i=0; i < sequences_remaining+sequence_nr; i++)
    delete [] robots_in_sequence[i];

  if( use_log_file && LOG_FILE ) LOG_FILE.close();
}

void
Arena::clear()
{
  delete_lists(true, true, true, true);

  for(int i=0; i < sequences_remaining+sequence_nr; i++)
    delete [] robots_in_sequence[i];

  sequence_nr = 0;
  sequences_remaining = 0;
  
  state = NOT_STARTED;
}

void 
Arena::interrupt_tournament()
{
  if( state == GAME_IN_PROGRESS )
    {
#ifndef NO_GRAPHICS
      if( !no_graphics )
        {
          the_gui.close_score_window();
          the_gui.close_arena_window();
          the_gui.close_message_window();
        }
#endif

      delete_lists(true, true, false, true);
      
      state = FINISHED;
    }
}

#ifndef NO_GRAPHICS

void
Arena::set_colours()
{  
  background_colour = make_gdk_colour(the_opts.get_l(OPTION_BACKGROUND_COLOUR));
  foreground_colour = make_gdk_colour(the_opts.get_l(OPTION_FOREGROUND_COLOUR));
}

#endif

void
Arena::set_filenames(String& log_fname, const String& statistics_fname, 
                     const String& tournament_fname, const String& option_fname)
{
  if( log_fname == "")
    {
      use_log_file = false;
    }
  else if( log_fname == "-" )  // use stdout as log_file
    {
      LOG_FILE.attach(STDOUT_FILENO);
      use_log_file = true;
    }
  else
    {
      LOG_FILE.open(log_fname.chars(), ios::out, 0600);
      use_log_file = true;
      if( !LOG_FILE )
        {
          cerr << "RealTimeBattle: Error: Couldn't open log_file" << endl;
          use_log_file = false;
        }
    }
  
  statistics_file_name = statistics_fname;

  tournament_file_name = tournament_fname;
  auto_start_and_end = ( tournament_file_name != "" );

  option_file_name = option_fname;
}

// This function takes the statistics and saves into a selected file
void
Arena::save_statistics_to_file(String filename)
{
  int mode = _IO_OUTPUT;
  ofstream file(filename.chars(), mode);

  GList * gl, * stat_gl;
  Robot * robotp;

  for(gl = g_list_next(all_robots_in_tournament); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot *)gl->data;
      file << robotp->get_robot_name() << ": " << endl;
      for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
        {
          stat_t * statp = (stat_t*)(stat_gl->data);
          file << "Seq: " << statp->sequence_nr << "  Game: " << statp->game_nr << "  Pos: " << statp->position
               << "  Points: " << statp->points << "  Time Survived: " << statp->time_survived
               << "  Total Points: " << statp->total_points << endl;
        }
    }
}

void
Arena::search_directories( String directory, GList* dir_gl,
                           const bool check_robots )
{
  DIR* dir;
  if( NULL != ( dir = opendir(directory.chars()) ) )
    {
      struct dirent* entry;
      while( NULL != ( entry = readdir( dir ) ) )
        {
          String full_file_name = directory + entry->d_name;
          bool res = false;
          if(check_robots)
            res = check_if_filename_is_robot(full_file_name);
          else
            res = check_if_filename_is_arena(full_file_name);
          if(res)
            {
              start_tournament_glist_info_t* info;
              info = new start_tournament_glist_info_t(0,false,full_file_name,"");
              g_list_append(dir_gl,info);
            }
        }
    }
}

void
Arena::check_for_robots_and_arenas( String& word, GList* tour_gl,
                                    GList* dir_gl, const bool check_robots )
{
  bool found = false;
  String full_file_name = "";

  if( word.get_length() > 1 )
    if( get_segment( word, -2, -1 ) == "/*" )
      {
        search_directories( get_segment( word, 0, -2 ), tour_gl, check_robots );
        return;
      }
  if( word.get_length() == 1 && word[0] == '*' )
    {
      GList* gl;
      for(gl=g_list_next(dir_gl);gl != NULL; gl=g_list_next(gl))
        search_directories( *((String*)gl->data), tour_gl, check_robots );
      return;
    }
  if( word.find('/') != -1 )
    {
      if((check_robots && check_if_filename_is_robot( word )) ||
         (!check_robots && check_if_filename_is_arena( word )))
        {
          full_file_name = word;
          found = true;
        }
    }
  if( !found )
    {
      GList* gl;
      for(gl=g_list_next(dir_gl);gl != NULL; gl=g_list_next(gl))
        {
          String* current_dir = (String*)gl->data;

          String temp_name = *current_dir + word;

          if((check_robots && check_if_filename_is_robot( temp_name )) ||
             (!check_robots && check_if_filename_is_arena( temp_name )))
            {
              full_file_name= temp_name;
              found = true;
              break;
            }
        }
    }
  if( found )
    {
      start_tournament_glist_info_t* info;
      info = new start_tournament_glist_info_t(0,false,full_file_name,"");
      g_list_append(tour_gl, info);
    }
  else
    {
      if(check_robots)
        cerr << "Couldn't find an executable robot with filename " << word << endl;
      else
        cerr << "Couldn't find an arena with filename " << word << endl;
    }
}

void
Arena::parse_tournament_file( String& fname )
{
  GList* robotdirs;
  GList* arenadirs;

  read_dirs_from_system(robotdirs, arenadirs);

  ifstream file(fname.chars());
  if( !file )
    return; // Don't know yet what to do when it isn't possible to find the tournament file

  int games_p_s = 1;
  int robots_p_s = 2;
  int n_o_sequences = 1;
  int looking_for = 0; // 0 = keyword, 1 = robot, 2 = arena

  GList* robot_glist = g_list_alloc();
  GList* arena_glist = g_list_alloc();

  for(;;)
    {
      char buffer[200];
      file >> buffer;
      String word(buffer);

      if( word == "" )
        {
          int robots_counted = 0;
          int arenas_counted = 0;

          GList* gl;
          for(gl=g_list_next(robot_glist); gl != NULL; gl=g_list_next(gl))
            robots_counted++;
          for(gl=g_list_next(arena_glist); gl != NULL; gl=g_list_next(gl))
            arenas_counted++;


          if (games_p_s == -1)
            games_p_s = arenas_counted;
          if (robots_p_s == -1)
            robots_p_s = robots_counted;
          if (n_o_sequences == -1)
            n_o_sequences=binomial(robots_counted, games_p_s);

          robots_p_s = min(robots_counted,robots_p_s);
          
          if(robots_p_s < 2)
              Error(true, "Can't start tournament with only " + String(robots_p_s) + " robots per sequence",
                    "Arena::parse_tournament_file");
          if(games_p_s < 1)
            Error(true, "Must have at least one game per sequence. Current value is: " + String(games_p_s),
                  "Arena::parse_tournament_file");
          if(n_o_sequences < 1)
            Error(true, "Must have at least one sequence. Current value is: " + String(n_o_sequences),
                  "Arena::parse_tournament_file");
          
          // Startup the tournament

          start_tournament( robot_glist , arena_glist, robots_p_s, games_p_s, n_o_sequences);  

          // Finally, delete the GLists

          for(gl=g_list_next(robot_glist); gl != NULL; gl=g_list_next(gl))  
            delete (start_tournament_glist_info_t*)(gl->data);
          g_list_free(robot_glist);

          for(gl=g_list_next(arena_glist); gl != NULL; gl=g_list_next(gl))  
            delete (start_tournament_glist_info_t*)(gl->data);
          g_list_free(arena_glist);

          clean_dir_glists(robotdirs, arenadirs);
          
          return;
        }
      if((make_lower_case(word) == "games/sequence:") || (make_lower_case(word) == "g/s:"))
        {
          looking_for = 0;
          file >> buffer;
          if( buffer[0] == '*' )
            games_p_s = -1;
          else
            games_p_s = str2int( buffer );
        }
      else if((make_lower_case(word) == "robots/sequence:") || (make_lower_case(word) == "r/s:"))
        {
          looking_for = 0;
          file >> buffer;
          if( buffer[0] == '*' )
            robots_p_s = -1;
          else
            robots_p_s = str2int( buffer );
        }
      else if((make_lower_case(word) == "sequences:") || (make_lower_case(word) == "seq:"))
        {
          looking_for = 0;
          file >> buffer;
          if( buffer[0] == '*' )
            n_o_sequences = -1;
          else
            n_o_sequences = str2int( buffer );
        }
      else if((make_lower_case(word) == "robots:") || (make_lower_case(word) == "r:"))
        looking_for = 1;
      else if((make_lower_case(word) == "arenas:") || (make_lower_case(word) == "a:"))
        looking_for = 2;
      else
        {
          switch(looking_for)
            {
            case 0:
              looking_for = 0;
              cerr << "Unrecognized keyword in tournament file: " << word << endl;
              break;
            case 1:
              check_for_robots_and_arenas( word, robot_glist, robotdirs, true);
              break;
            case 2:
              check_for_robots_and_arenas( word, arena_glist, arenadirs, false);
              break;
            }
        }
    }
}

void
Arena::print_to_logfile(const char first_letter ... )
{
  if( !use_log_file ) return;

  va_list args;
  va_start(args, first_letter);
  LOG_FILE << first_letter;

  LOG_FILE << setiosflags(ios::fixed) << setprecision(2);

  switch(first_letter)
    {
    case 'R': // Robot position info
      LOG_FILE << va_arg(args, int   ) << " ";     // robot id;
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double) << " ";  // y
      LOG_FILE << va_arg(args, double) << " ";  // cannon angle
      LOG_FILE << va_arg(args, double) << " ";  // radar angle
      LOG_FILE << va_arg(args, double);         // energy
      break;

    case 'T': // Time
      LOG_FILE << va_arg(args, double);  // time elsaped
      break;

    case 'P': // Print a robot message
      LOG_FILE << va_arg(args, int   ) << " ";  // robot id
      LOG_FILE << va_arg(args, char* );         // message to print
      break;

    case 'C': // Cookie
      LOG_FILE << va_arg(args, int   ) << " ";  // cookie id
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double);         // y
      break;

    case 'M': // Mine
      LOG_FILE << va_arg(args, int   ) << " ";  // mine id
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double);         // y
      break;

    case 'S': // Shot
      LOG_FILE << va_arg(args, int   ) << " ";  // shot id
      LOG_FILE << va_arg(args, double) << " ";  // x
      LOG_FILE << va_arg(args, double) << " ";  // y
      LOG_FILE << va_arg(args, double) << " ";  // dx/dt
      LOG_FILE << va_arg(args, double);         // dy/dt
      break;
      
    case 'D': // Die
      {
        char obj_type = va_arg(args, char  );
        LOG_FILE << obj_type          << " ";  // object type to kill
        LOG_FILE << va_arg(args, int) << " ";  // object id
        if( obj_type == 'R' )
          LOG_FILE << va_arg(args, double);    // robot points received
      }
      break;

    case 'G': // Game begins
      LOG_FILE << va_arg(args, int  ) << " ";  // sequence number
      LOG_FILE << va_arg(args, int  ) << " ";  // game number
      LOG_FILE << va_arg(args, char*);         // arena filename
      break;

    case 'H': // Header
      LOG_FILE << va_arg(args, int  ) << " ";  // number of games per sequence
      LOG_FILE << va_arg(args, int  ) << " ";  // number of robots per sequence
      LOG_FILE << va_arg(args, int  ) << " ";  // number of sequences
      LOG_FILE << va_arg(args, int  ) << " ";  // number of robots
      LOG_FILE << va_arg(args, char*);         // name of optionfile
      break;

    case 'L': // List of robot properties
      LOG_FILE << va_arg(args, int  ) << " ";  // robot id
      LOG_FILE << hex2str(va_arg(args, long )) << " ";  // robot colour
      LOG_FILE << va_arg(args, char*);  // robot name
      break;

    default:
      Error(true, "Unrecognized first letter in logfile", "Arena::print_to_logfile");
      break;
    }

  LOG_FILE << endl;
}


void
Arena::parse_file(istream& file)
{
  char text[20];
  double radie, bounce_c, hardn, thickness;
  int vertices;

  Vector2D vec1, vec2, vec0;
  WallLine* wall_linep;
  WallCircle* wall_circlep;
  WallInnerCircle* wall_inner_circlep;

  int succession = 1;
  double scale = the_opts.get_d(OPTION_ARENA_SCALE);
  do
    {
      file >> ws;
      file.get(text, 20, ' ');
      if( strcmp(text, "scale" ) == 0 )
        {
          if( succession != 1 ) Error(true, "Error in arenafile: 'scale' not first", "Arena::parsefile");
          succession = 2;
          double scl;
          file >> scl;
          scale *= scl;
        }
      else if( strcmp(text, "boundary" ) == 0 )
        {
          if( succession > 2 ) 
            Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate", "Arena::parsefile");
          succession = 3;
          double b1, b2;
          file >> b1;
          file >> b2;
          boundary[0] = Vector2D(scale*b1, scale*b2);
          file >> b1;
          file >> b2;
          boundary[1] = Vector2D(scale*b1, scale*b2);
          if( boundary[1][0] - boundary[0][0] <= 0 || boundary[1][1] - boundary[0][1] <= 0 ) 
            Error(true, "Error in arenafile: 'boundary' negative", "Arena::parsefile");
        }
      else if( strcmp(text, "exclusion_point" ) == 0 )
        {
          if( succession < 3 ) 
            Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate", "Arena::parsefile");
          file >> vec1;
          Vector2D* excl_p = new Vector2D(scale*vec1);
          g_list_append(exclusion_points, excl_p);
        }
      else if( strcmp(text, "inner_circle" ) == 0 )
        {
          if( succession < 3 ) Error(true, "Error in arenafile: 'inner_circle' before boundary", "Arena::parsefile");
          succession = 4;
          file >> bounce_c;
          file >> hardn;
          file >> vec1;
          file >> radie;
          wall_inner_circlep = new WallInnerCircle(scale*vec1, scale*radie, bounce_c, hardn);
          g_list_insert(object_lists[WALL], wall_inner_circlep, 1);
        }
      else if( strcmp(text, "circle" ) == 0 )
        {
          if( succession < 3 ) Error(true, "Error in arenafile: 'circle' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> bounce_c;
          file >> hardn;
          file >> vec1;
          file >> radie;
          wall_circlep = new WallCircle(scale*vec1, scale*radie, bounce_c, hardn);
          g_list_append(object_lists[WALL], wall_circlep);
        }
//       else if( strcmp(text, "arc" ) == 0 )
//         {
//         }
      else if( strcmp(text, "line" ) == 0 )
        {
          if( succession < 3 ) Error(true, "Error in arenafile: 'line' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> bounce_c;
          file >> hardn;
          file >> thickness; 
          thickness *= 0.5;
          file >> vec1;      // start_point
          file >> vec2;      // end_point


          if( length(vec2-vec1) == 0.0 ) Error(true, "Error in arenafile: Zero length line", "Arena::parsefile");

          wall_linep = new WallLine(scale*vec1, unit(vec2-vec1), scale*length(vec2-vec1), 
                                    scale*thickness, bounce_c , hardn);      
          g_list_append(object_lists[WALL], wall_linep);
        }
      else if( strcmp(text, "polygon" ) == 0 )
        {
          if( succession < 3 ) Error(true, "Error in arenafile: 'polygon' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> bounce_c;
          file >> hardn;
          file >> thickness;
          thickness *= 0.5;
          file >> vertices;   // number of vertices
          file >> vec1;      // first point
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          g_list_append(object_lists[WALL], wall_circlep);

          for(int i=1; i<vertices; i++)
            {
              vec2 = vec1;
              file >> vec1;      // next point

              if( length(vec2-vec1) == 0.0 ) Error(true, "Error in arenafile: Zero length line in polygon", "Arena::parsefile");

              wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), scale*length(vec1-vec2), 
                                        scale*thickness, bounce_c , hardn);      
              g_list_append(object_lists[WALL], wall_linep);
              wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
              g_list_append(object_lists[WALL], wall_circlep);
            }
        }
      else if( strcmp(text, "closed_polygon" ) == 0 )
        {
          if( succession < 3 ) Error(true, "Error in arenafile: 'closed_polygon' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> bounce_c;
          file >> hardn;
          file >> thickness;
          thickness *= 0.5;
          file >> vertices;   // number of vertices
          file >> vec1;      // first point
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          g_list_append(object_lists[WALL], wall_circlep);
          vec0 = vec1;

          for(int i=1; i<vertices; i++)
            {
              vec2 = vec1;
              file >> vec1;      // next point

              if( length(vec2-vec1) == 0.0 ) Error(true, "Error in arenafile: Line in closed_polygon of zero length", "Arena::parsefile");
          
              wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), scale*length(vec1-vec2), 
                                        scale*thickness, bounce_c , hardn);      
              g_list_append(object_lists[WALL], wall_linep);
              wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
              g_list_append(object_lists[WALL], wall_circlep);
            }

          if( length(vec0-vec1) == 0.0 ) Error(true, "Error in arenafile: Last line in closed_polygon of zero length", "Arena::parsefile");
          wall_linep = new WallLine(scale*vec1, unit(vec0-vec1), scale*length(vec0-vec1), 
                                    scale*thickness, bounce_c , hardn);      
          g_list_append(object_lists[WALL], wall_linep);
        }
      else if( text[0] != '\0' )
        Error(true, "Incorrect arenafile: unknown keyword" + (String)text, "Arena::parsefile");
        
    } while( text[0] != '\0' );
}

double
Arena::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, 
                             object_type& closest_shape, void*& colliding_object, const Robot* the_robot)
{
  double dist = infinity;
  double d;
  closest_shape = NOOBJECT;
  GList* gl;

  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    {
      if( (Robot*)gl->data != the_robot )
        {
          d = ((Robot*)gl->data)->get_distance(pos, dir, size);
          if( d < dist)
            {
              closest_shape = ROBOT;
              colliding_object = gl->data;
              dist = d;
            }
        }
    }
  for(gl = g_list_next(object_lists[WALL]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Shape*)(WallCircle*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = WALL;
          colliding_object = gl->data;
          dist = d;
        }
    }
  for(gl = g_list_next(object_lists[MINE]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Mine*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = MINE;
          colliding_object = gl->data;
          dist = d;
        }
    }
  
  for(gl = g_list_next(object_lists[COOKIE]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Cookie*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = COOKIE;
          colliding_object = gl->data;
          dist = d;
        }
    }

  for(gl = g_list_next(object_lists[SHOT]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Shot*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = SHOT;
          colliding_object = gl->data;
          dist = d;
        }
    }

  return dist;
}

bool
Arena::space_available(const Vector2D& pos, const double margin)
{
  GList* gl;

  for(gl=g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    if( ((Robot*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[WALL]); gl != NULL; gl = g_list_next(gl))
    if( ((Shape*)(WallCircle*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[MINE]); gl != NULL; gl = g_list_next(gl))
    if( ((Mine*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[COOKIE]); gl != NULL; gl = g_list_next(gl))
    if( ((Cookie*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[SHOT]); gl != NULL; gl = g_list_next(gl))
    if( ((Shot*)gl->data)->within_distance(pos, margin) ) return false;

  // Check if it is possible to see any exclusion_points
  
  Vector2D vec;
  double dist;
  object_type obj_t;
  void* col_obj;
  for(gl=g_list_next(exclusion_points); gl != NULL; gl = g_list_next(gl))
    {
      vec = *((Vector2D*)gl->data);
      dist = length(vec - pos);
      if( dist <= margin || 
          dist <= get_shortest_distance(pos, unit(vec - pos), 0.0, obj_t, col_obj) )
        return false;
    }

  return true;
}

Vector2D
Arena::get_random_position()
{
  return Vector2D( boundary[0][0] + (boundary[1][0] - boundary[0][0])*
                   (double)rand()/(double)RAND_MAX, 
                   boundary[0][1] + (boundary[1][1] - boundary[0][1])*
                   (double)rand()/(double)RAND_MAX );
}

double 
Arena::get_shooting_penalty() 
{ 
  return min( the_opts.get_d(OPTION_SHOOTING_PENALTY), 0.5 / (double)robots_left ); 
}

void
Arena::broadcast(const message_to_robot_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);
  String str = (String)message_to_robot[msg_type].msg + ' ';
  for(int i=0; i<message_to_robot[msg_type].number_of_args; i++)
    {
      switch(message_to_robot[msg_type].arg_type[i])
        {
        case NONE: 
          Error(true, "Couldn't send message, no arg_type", "Robot::send_message");
          break;
        case INT:
          str += (String)va_arg(args, int) + ' ';
          break;
        case DOUBLE:
          str += String(va_arg(args, double), 6) + ' ';
          break;
        case STRING:
          str += (String)va_arg(args, char*) + ' ';
          break;   
        case HEX:
          str += hex2str(va_arg(args, int)) + ' ';
          break;
        default:
          Error(true, "Couldn't send message, unknown arg_type", "Robot::send_message");
        }
    }
  str += '\n';

  for(GList* gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    *(((Robot*)gl->data)->get_outstreamp()) << str;
}

void
Arena::quit_ordered()
{
  state = EXITING;
}

gint
Arena::timeout_function()
{
  if( halted ) return true;
#ifndef NO_GRAPHICS
  int old_total = (int)total_time;
#endif

  update_timer ();

  switch(state)
    {
    case NOT_STARTED:
      if(auto_start_and_end)
        parse_tournament_file(tournament_file_name);
      break;

    case FINISHED:
      if(auto_start_and_end)
        {
          if( statistics_file_name != "" )
            save_statistics_to_file( statistics_file_name );
          gtk_main_quit();
        }
      break;
      
    case STARTING_ROBOTS:
      {
        read_robot_messages();

        if( total_time > next_check_time ) start_sequence_follow_up();
      }
      break;
      
    case SHUTTING_DOWN_ROBOTS:
      {     
        GList* gl;
        
        for(gl = g_list_next(all_robots_in_sequence); gl != NULL; gl = g_list_next(gl) )
          ((Robot*)gl->data)->get_messages();
        
        if( total_time > next_check_time ) end_sequence_follow_up();
      }
      break;
      
    case GAME_IN_PROGRESS:
      {
        update();
#ifndef NO_GRAPHICS
        if((int)total_time > old_total && !no_graphics) the_gui.set_score_window_title();
#endif
        if( robots_left <= 1 || total_time > the_opts.get_d(OPTION_TIMEOUT) ) 
          {
            end_game();
          }
        else
          {
            if( game_mode == COMPETITION_MODE && total_time > next_check_time ) check_robots();
            
            // Place mines and cookies
            if( ((double)rand()) / (double)RAND_MAX <= timestep*the_opts.get_d(OPTION_COOKIE_FREQUENCY) )
              add_cookie();
            
            if( ((double)rand()) / (double)RAND_MAX <= timestep*the_opts.get_d(OPTION_MINE_FREQUENCY) )
              add_mine();
          }
      }
      break;
    case PAUSING_BETWEEN_GAMES:
      if( !paus_after_next_game ) start_game();
      break;

    case EXITING:
      return false;
      
    case NO_STATE:
      Error(true, "Arena state is NO_STATE, shouldn't ever happen!", "Arena::timeout_function");
    }

  if( halt_next )
    {
      halted = true;
      halt_next = false;
    }
  return true;
}

void
Arena::update()
{
  print_to_logfile('T', total_time);

  read_robot_messages();
  move_shots();
  update_robots();

#ifndef NO_GRAPHICS
  if( state == GAME_IN_PROGRESS && !no_graphics)
    the_gui.draw_objects(true);
#endif
}

void
Arena::update_timer()
{
  gdouble last_timer = current_timer;
  gulong microsecs;
  current_timer = g_timer_elapsed(timer, &microsecs);
  gdouble timescale = 1.0;
  if( state == GAME_IN_PROGRESS ) timescale = the_opts.get_d(OPTION_TIMESCALE);
  timestep = min( (current_timer - last_timer) * timescale, the_opts.get_d(OPTION_MAX_TIMESTEP) );
  total_time += timestep;
}

void
Arena::reset_timer()
{
  total_time = 0.0;
  current_timer = 0.0;
  g_timer_reset(timer);
  update_timer();
}

void
Arena::add_cookie()
{
  double en = the_opts.get_d(OPTION_COOKIE_MIN_ENERGY) + 
    (the_opts.get_d(OPTION_COOKIE_MAX_ENERGY) - the_opts.get_d(OPTION_COOKIE_MIN_ENERGY)) * 
    (double)rand() / (double)RAND_MAX;
  bool found_space = false;
  double r = the_opts.get_d(OPTION_COOKIE_RADIUS);
  Vector2D pos;

  for( int i=0; i<100 && !found_space; i++)
    {
      pos = get_random_position();
      found_space = space_available(pos, r*2.0);
    }
  
  if( !found_space ) Error(false, "Couldn't find space for cookie", "Arena::timeout_function");
  Cookie* cookiep = new Cookie(pos, r, en);
  g_list_append(object_lists[COOKIE], cookiep);

  print_to_logfile('C', cookiep->get_id(), pos[0], pos[1]);
}

void
Arena::add_mine()
{
  double en = the_opts.get_d(OPTION_MINE_MIN_ENERGY) + 
    (the_opts.get_d(OPTION_MINE_MAX_ENERGY) - the_opts.get_d(OPTION_MINE_MIN_ENERGY)) * 
    (double)rand() / (double)RAND_MAX;
  bool found_space = false;
  double r = the_opts.get_d(OPTION_MINE_RADIUS);
  Vector2D pos;

  for( int i=0; i<100 && !found_space; i++)
    {
      pos = get_random_position();
      found_space = space_available(pos, r*2.0);
    }
  
  if( !found_space ) Error(false, "Couldn't find space for mine", "Arena::timeout_function");
  Mine* minep = new Mine(pos, r, en);
  g_list_append(object_lists[MINE], minep);

  print_to_logfile('M', minep->get_id(), pos[0], pos[1]);
}

void
Arena::check_robots()
{
  Robot* robotp;

  GList* gl = g_list_next(all_robots_in_sequence); 
  for( ; gl != NULL; gl=g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      if( robotp->is_process_running() )
        {
          robotp->check_process();
        }      
    } 

  next_check_time = total_time + the_opts.get_d(OPTION_CHECK_INTERVAL);
}

void
Arena::move_shots()
{
  GList* gl;
  Shot* shotp;

  for(gl = g_list_next(object_lists[SHOT]); gl != NULL; )
    {
      shotp = (Shot*)gl->data;

      if( shotp->is_alive() ) shotp->move(timestep);

      gl = g_list_next(gl);
      if( !shotp->is_alive() ) 
        {
          g_list_remove(object_lists[SHOT], shotp);
          delete shotp;
        }
    }
}

void
Arena::read_robot_messages()
{
  GList* gl;
  Robot* robotp;

  for(gl = g_list_next(all_robots_in_sequence); gl != NULL; gl = g_list_next(gl) )
    {
      robotp = (Robot*)gl->data;
      if( robotp->is_alive() || state != GAME_IN_PROGRESS )  
        robotp->get_messages();
    }
}

void
Arena::update_robots()
{
  GList* gl;
  Robot* robotp;

  int killed_robots = 0;
  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl) )
    {
      robotp = (Robot*)gl->data;
      if( robotp->is_alive() )
        {
          robotp->update_radar_and_cannon(timestep);  
          robotp->change_velocity(timestep);
          robotp->move(timestep);        
        }
      if( robotp->is_alive() ) 
        robotp->get_messages();
    }

  // Check if robots have died and send energy level

  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; )
    {
      robotp = (Robot*)gl->data;
      gl = g_list_next(gl);
      if( !robotp->is_alive() ) 
        {
          g_list_remove(object_lists[ROBOT], robotp);          
          killed_robots++;
        }
      else
        {
          double lvls = (double)the_opts.get_l(OPTION_ROBOT_ENERGY_LEVELS);
          robotp->send_message( ENERGY, rint( robotp->get_energy() / lvls ) * lvls );
        }
    }

  if( killed_robots > 0 )
    {
      for( gl=g_list_next(object_lists[ROBOT]); gl != NULL; gl=g_list_next(gl))
        {
          ((Robot*)gl->data)->add_points(killed_robots);
#ifndef NO_GRAPHICS
          if( robots_left < 15 && !no_graphics ) ((Robot*)gl->data)->display_score();
#endif
        }

      for( gl=g_list_next(all_robots_in_sequence); gl != NULL; gl=g_list_next(gl))
        if( ((Robot*)gl->data)->get_position_this_game() == -1 )
          ((Robot*)gl->data)->set_stats(killed_robots);
      
      robots_left -= killed_robots;
      broadcast(ROBOTS_LEFT, robots_left);
    }

  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl=g_list_next(gl))
    ((Robot*)gl->data)->send_signal();
}

double
Arena::colour_dist(const long col1, const GdkColor& col2)
{
  return( ((double)abs((col1 & 0xff)*0x101 - col2.blue))*
          log(1.0 + sqrt((col1 & 0xff)*0x101 + col2.blue))/log(2.0) +
          ((double)abs(((col1 & 0xff00) >> 8)*0x101 - col2.green))*
          log(1.0 + sqrt(((col1 & 0xff00) >> 8)*0x101 + col2.green))/log(2.0) +
          ((double)abs(((col1 & 0xff0000) >> 16)*0x101 - col2.red))*
          log(1.0 + sqrt(((col1 & 0xff0000) >> 16)*0x101 + col2.red))/log(2.0));
}

bool
Arena::is_colour_allowed(const long colour, const double min_dist, const Robot* robotp)
{
  double d;
  for(GList* gl = g_list_next(all_robots_in_sequence); gl != NULL; gl = g_list_next(gl))
    {
      if((Robot *)gl->data != robotp)
        {
          d = colour_dist( colour, ((Robot*)gl->data)->get_colour() );
          if( d < min_dist ) return false;          
        }
    }
  
  d = colour_dist( colour, background_colour );
  if( d < min_dist ) return false;

  return true;
}

long
Arena::find_free_colour(const long home_colour, const long away_colour, const Robot* robotp)
{  
  long tmp_colour;

  for(double min_dist = 200000.0; min_dist > 0.5 ; min_dist *= 0.8)
    {
      if( is_colour_allowed(home_colour, min_dist, robotp) ) return home_colour;
      if( is_colour_allowed(away_colour, min_dist, robotp) ) return away_colour;
      for(int i=0; i<25; i++)
        {
          tmp_colour = rand() & 0xffffff;
          if( is_colour_allowed(tmp_colour, min_dist*2, robotp) ) return tmp_colour;
        }                  
    }
   Error(true, "Impossible to find colour", "Arena::find_free_colour");
}

void 
Arena::set_game_mode( const enum game_mode_t gm)
{
  game_mode = gm; 
  if( game_mode == DEBUG_MODE )
    {
      if( debug_level == 0 ) debug_level = 1;
    }
  else
    {
      debug_level = 0;
    }
}

int
Arena::set_debug_level( const int new_level)
{
  if( new_level > max_debug_level || new_level < 0 || new_level == debug_level ) return debug_level;
  
  debug_level = new_level;

  if( GAME_IN_PROGRESS )
    {
      broadcast(GAME_OPTION, DEBUG_LEVEL, (double)debug_level);
    }

  return debug_level;
}

void
Arena::paus_game_toggle()
{
  if( game_mode != COMPETITION_MODE )
    {
      halted = !halted; 
      halt_next = false; 
    }
  else
    {
      paus_after_next_game = !paus_after_next_game;
    }
}

void
Arena::step_paused_game()
{
  if( game_mode == DEBUG_MODE && halted )
    {
      halt_next = true; 
      halted = false;
    }
}

bool
Arena::is_game_halted()
{
  return( state == PAUSING_BETWEEN_GAMES || 
          ( game_mode != COMPETITION_MODE && halted ) );
}

void
Arena::delete_lists(const bool kill_robots, const bool del_seq_list, 
                    const bool del_tourn_list, const bool del_arena_filename_list)
{
  GList* gl;
  // clear the lists;
  Robot* robotp;
  for(gl=g_list_next(object_lists[ROBOT]); gl != NULL; )
    {
      robotp = (Robot*)gl->data;
      gl=g_list_next(gl);
      g_list_remove(object_lists[ROBOT], robotp);
    }
  Shot* shotp;
  for(gl=g_list_next(object_lists[SHOT]); gl != NULL; )
    {
      shotp = (Shot*)gl->data;
      delete shotp;
      gl=g_list_next(gl);
      g_list_remove(object_lists[SHOT], shotp);
    }
  Mine* minep;
  for(gl=g_list_next(object_lists[MINE]); gl != NULL; )
    {
      minep = (Mine*)(gl->data); 
      delete minep;
      gl=g_list_next(gl);
      g_list_remove(object_lists[MINE], minep);
    }
  Cookie* cookiep;
  for(gl=g_list_next(object_lists[COOKIE]); gl != NULL; )
    {
      cookiep = (Cookie*)(gl->data);
      delete cookiep;
      gl=g_list_next(gl);
      g_list_remove(object_lists[COOKIE], cookiep);
    }
  void* wallp;
  for(gl=g_list_next(object_lists[WALL]); gl != NULL; )
    {
      wallp = gl->data;
      delete (Shape*)(WallCircle*)wallp;
      gl=g_list_next(gl);
      g_list_remove(object_lists[WALL], wallp);
    }
  Vector2D* vecp;
  for(gl=g_list_next(exclusion_points); gl != NULL; )
    {
      vecp = (Vector2D*)gl->data;
      gl=g_list_next(gl);
      delete vecp;
      g_list_remove(exclusion_points, vecp);
    }
  if( del_seq_list )
    for(gl=g_list_next(all_robots_in_sequence); gl != NULL; )
      {
        robotp = (Robot*)gl->data;
        gl=g_list_next(gl);
        if( kill_robots ) robotp->kill_process_forcefully();
        g_list_remove(all_robots_in_sequence, robotp);
      }
  if( del_tourn_list )
    for(gl=g_list_next(all_robots_in_tournament); gl != NULL; )
      {
        robotp = (Robot*)gl->data;
        gl=g_list_next(gl);
        delete robotp;
        g_list_remove(all_robots_in_tournament, robotp);
      }
  if( del_arena_filename_list )
    {
      String* stringp;
      for(gl=g_list_next(arena_filenames); gl != NULL; )
        {
          stringp = (String*)gl->data;
          gl=g_list_next(gl);
          delete stringp;
          g_list_remove(arena_filenames, stringp);
        }
    }
}

void
Arena::start_game()
{
  // put the arena together

  if( paus_after_next_game )
    {
      state = PAUSING_BETWEEN_GAMES;
      return;
    }
  
  current_arena_nr = current_arena_nr % number_of_arenas + 1;
  
  String* filename = (String*)g_list_nth(arena_filenames, current_arena_nr)->data;
  ifstream file(filename->chars());
  if( !file ) Error(true, "Couldn't open arena file" + *filename, "Arena::start_game");

  parse_file(file);

  int charpos;
  if( (charpos = filename->find('/',0,true)) != -1 )
    current_arena_filename = get_segment(*filename, charpos+1, -1);
  else
    Error(true, "Incomplete arena file path" + *filename, "Arena::start_game");

  // reset some variables

  shot_count = 0;
  cookie_count = 0;
  mine_count = 0;


  // Place robots on the arena

  GList* gl = g_list_next(all_robots_in_sequence);
  Robot* robotp;
  bool found_space;
  double angle;
  Vector2D pos;

  robots_left = 0;
  for( ; gl != NULL; gl=g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      found_space = false;
      for( int i=0; i<100 && !found_space; i++)
        {
          pos = get_random_position();
          found_space = space_available(pos, the_opts.get_d(OPTION_ROBOT_RADIUS)*1.2);
        }

      if( !found_space ) Error(true, "Couldn't find space for all robots", "Arena::start_game");
      angle = ((double)rand())*2.0*M_PI/RAND_MAX;
      robotp->set_values_before_game(pos, angle);
      g_list_append(object_lists[ROBOT], gl->data);
      robots_left++;
      ((Robot*)gl->data)->live();
    }

  print_to_logfile('G', sequence_nr, games_per_sequence - games_remaining_in_sequence + 1, 
                   current_arena_filename.chars());

  broadcast(GAME_STARTS);
  broadcast(ROBOTS_LEFT, robots_left);
  the_opts.broadcast_opts();

  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl=g_list_next(gl))
    ((Robot*)gl->data)->send_signal();

  state = GAME_IN_PROGRESS;
  games_remaining_in_sequence--;

#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      the_gui.clear_area();
      the_gui.change_zoom();
      the_gui.add_robots_to_score_list();

      reset_timer();  // Time should be zero in score window
      the_gui.set_score_window_title();
      the_gui.set_arena_window_title();
    }
#endif

  reset_timer();  // Game starts !
  next_check_time = total_time + the_opts.get_d(OPTION_CHECK_INTERVAL);
}

void
Arena::end_game()
{
  for(GList* gl=g_list_next(all_robots_in_sequence); gl != NULL; gl=g_list_next(gl))
    if( ((Robot*)gl->data)->get_position_this_game() == 0 )
      {
        ((Robot*)gl->data)->die();
        ((Robot*)gl->data)->set_stats(1);
      }

  broadcast(GAME_FINISHES);

  delete_lists(false, false, false, false);
  
  if(games_remaining_in_sequence == 0) 
    end_sequence();
  else
    start_game();
}


void
Arena::start_sequence()
{
  games_remaining_in_sequence = games_per_sequence;
  current_arena_nr = 0;

  // Make list of robots in this sequence

  for(int i=0; i<robots_per_game; i++)
    {
      g_list_append( all_robots_in_sequence, 
                     g_list_nth(all_robots_in_tournament, 
                                robots_in_sequence[sequence_nr][i])->data );
    }

  // execute robot processes
  GList* gl = g_list_next(all_robots_in_sequence);

  for(; gl != NULL; gl = g_list_next(gl))
    {
      ((Robot*)gl->data)->start_process();
    }
  
  // wait a second before checking

  state = STARTING_ROBOTS;
  sequence_nr++;
  sequences_remaining--;
  reset_timer();
  next_check_time = total_time + 1.0;
}

void
Arena::start_sequence_follow_up()
{
  // check if the process have started correctly
  GList* gl = g_list_next(all_robots_in_sequence);
  Robot* robotp;

  for(; gl != NULL; )
    {
      robotp = (Robot*)gl->data;
      gl = g_list_next(gl);
      if( !(robotp->is_process_running()) ) 
        {
          g_list_remove(all_robots_in_sequence, robotp);
          robots_left--;
        }
      if( !robotp->set_and_get_have_competed() )
        print_to_logfile('L', robotp->get_id(), gdk2hex_colour(robotp->get_colour()), robotp->get_robot_name().chars());
    }
  start_game();
}

void
Arena::end_sequence()
{
  // kill all robot processes
  
  GList* gl = g_list_next(all_robots_in_sequence);

  for(; gl != NULL; gl = g_list_next(gl))
    {
      ((Robot*)gl->data)->end_process();
    }

  // wait a second before checking

  state = SHUTTING_DOWN_ROBOTS;
  next_check_time = total_time + 1.0;
}

void
Arena::end_sequence_follow_up()
{
  // check if the process have stopped, otherwise kill
  
  GList* gl = g_list_next(all_robots_in_sequence);
  Robot* robotp;

  for(; gl != NULL; )
    {
      robotp = (Robot*)gl->data;
      gl = g_list_next(gl);
      if( robotp->is_process_running() ) robotp->kill_process_forcefully();
      robotp->delete_pipes();
      g_list_remove(all_robots_in_sequence, robotp);
    }

  if(sequences_remaining == 0) 
    end_tournament();
  else
    start_sequence();
}

void
Arena::start_tournament(const GList* robotfilename_list, const GList* arenafilename_list, 
                        int robots_p_game, int games_p_sequence, int n_o_sequences)
{
  clear();

  // Create robot classes and to into the list all_robots_in_tournament

#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      the_gui.setup_message_window();
      the_gui.setup_arena_window();
      the_gui.setup_score_window();
    }
#endif

  number_of_robots = 0;
  robot_count = 0;
  Robot* robotp;
  start_tournament_glist_info_t* infop;
  String* stringp;

  GList* gl;
  for(gl = g_list_next(robotfilename_list); gl != NULL; gl = g_list_next(gl))
    {
      infop = (start_tournament_glist_info_t*)gl->data;
      robotp = new Robot(infop->filename);
      g_list_append(all_robots_in_tournament, robotp);
      number_of_robots++;
    }

  // Create list of arena filenames
  number_of_arenas = 0;

  for(gl = g_list_next(arenafilename_list); gl != NULL; gl = g_list_next(gl))
    {
      stringp = new String(((start_tournament_glist_info_t*)gl->data)->filename);
      g_list_append(arena_filenames, stringp);
      number_of_arenas++;
    }

  robots_per_game = robots_p_game;
  games_per_sequence = games_p_sequence;
  sequences_remaining = n_o_sequences;

  // make list of robots to compete in the sequences
  int games_per_round = binomial(number_of_robots, robots_per_game);
  int complete_rounds = n_o_sequences / games_per_round;
  int rem_games = n_o_sequences % games_per_round;

  robots_in_sequence = new (int*)[n_o_sequences];
  for(int i=0; i<n_o_sequences; i++) robots_in_sequence[i] = new int[robots_per_game];
  
  int current_sequence[robots_per_game];
  int current_nr = 0;
  //  for(int i=0; i<robots_per_game; i++) current_sequence[i] = i+1;
  
  // set complete rounds first

  for(int round_nr=0; round_nr < complete_rounds; round_nr++)
    {
      int k, i, j;

      for(i=0; i<robots_per_game; i++) current_sequence[i] = i+1;
      current_sequence[robots_per_game-1]--;   // To be increased first time

      
      for(i=0; i< games_per_round; i++)
        {
          
          k = robots_per_game - 1;
          while( current_sequence[k] == number_of_robots + 1 - robots_per_game + k )
            k--;

          if( k < 0 ) Error(true, "Problem generating list of participants, k < 0", 
                            "Arena::start_tournament");

          current_sequence[k]++;
          for(j=k+1; j<robots_per_game; j++) current_sequence[j] = current_sequence[j-1]+1;

          for(j=0; j<robots_per_game; j++) 
            robots_in_sequence[current_nr][j] = current_sequence[j];

          current_nr++;
        }
      reorder_pointer_array((void**)robots_in_sequence, games_per_round);
    }

  // the remaining game will be randomly chosen

  int robot_matches_played[number_of_robots];
  for(int i=0; i<number_of_robots; i++) robot_matches_played[i] = 0;

  bool robot_playing_this_match[number_of_robots];
  int min_matches_played = 0;
  int number_of_robots_on_min_matches = number_of_robots;
  int nr;

  for(int i=0; i< rem_games; i++)
    {
      for(int i2=0; i2<number_of_robots; i2++) robot_playing_this_match[i2] = false;

      for(int j=0; j<robots_per_game; j++)
        {
          do 
            nr = (int)floor(number_of_robots*((double)rand() / (double)RAND_MAX));
          while( robot_playing_this_match[nr] || robot_matches_played[nr] != min_matches_played );

          robot_playing_this_match[nr] = true;
          robot_matches_played[nr]++;
          number_of_robots_on_min_matches--;
          if( number_of_robots_on_min_matches == 0 ) 
            {
              min_matches_played++;
              number_of_robots_on_min_matches = number_of_robots;
            }

          robots_in_sequence[current_nr][j] = nr + 1;   // robot count will start from 1
        }      
      current_nr++;
    }

  // set random seed

  gulong usecs;
  g_timer_elapsed(timer, &usecs); 
  srand(usecs);

  // start first sequence

  print_to_logfile('H', games_per_sequence, robots_per_game, sequences_remaining, 
                   number_of_robots, option_file_name.chars());

  sequence_nr = 0;
  start_sequence();
}

void
Arena::end_tournament()
{
  state = FINISHED;

#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      the_gui.close_message_window();
      the_gui.close_score_window();
      the_gui.close_arena_window();
      //  the_gui.close_control_window();
    }
#endif
}
