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

//#include "Gui.h"
#include "Arena_Base.h"
//#include "MovingObject.h"
//#include "Shape.h"
#include "Extras.h"
#include "Various.h"
#include "String.h"
#include "Shot.h"
#include "Options.h"
#include "Wall.h"
#include "Robot.h"

Arena_Base::Arena_Base()
{
  state = NOT_STARTED;
  game_mode = NORMAL_MODE;

  sequence_nr = 0;
  sequences_remaining = 0;
  
  halted = false;
  halt_next = false;
  paus_after_next_game = false;

  reset_timer();
  
  all_robots_in_sequence = g_list_alloc();
  all_robots_in_tournament = g_list_alloc();
  arena_filenames = g_list_alloc();
  exclusion_points = g_list_alloc();

  for(int i=ROBOT_T; i<=LAST_ARENAOBJECT_T; i++)
    object_lists[i] = g_list_alloc();

  max_debug_level = 5;
  debug_level = 0;
}

Arena_Base::~Arena_Base()
{
  //  if( filep ) delete filep;
  state=EXITING;
  sleep(1);

  delete_lists(true, true, true, true);
  g_list_free(all_robots_in_sequence);
  g_list_free(all_robots_in_tournament);
  g_list_free(arena_filenames);
  g_list_free(exclusion_points);
  for(int i=ROBOT_T; i<LAST_ARENAOBJECT_T; i++)
    g_list_free(object_lists[i]);

  for(int i=0; i < sequences_remaining+sequence_nr; i++)
    delete [] robots_in_sequence[i];
}

void
Arena_Base::clear()
{
  delete_lists(true, true, true, true);

  for(int i=0; i < sequences_remaining+sequence_nr; i++)
    delete [] robots_in_sequence[i];

  sequence_nr = 0;
  sequences_remaining = 0;
  
  state = NOT_STARTED;
}

void 
Arena_Base::interrupt_tournament()
{
  if( state == GAME_IN_PROGRESS )
    {
#ifndef NO_GRAPHICS
      if( !no_graphics )
        {
          the_gui.close_scorewindow();
          the_gui.close_arena_window();
          the_gui.close_messagewindow();
        }
#endif

      delete_lists(true, true, false, true);
      
      state = FINISHED;
    }
}

#ifndef NO_GRAPHICS

void
Arena_Base::set_colours()
{  
  background_colour = make_gdk_colour(the_opts.get_l(OPTION_BACKGROUND_COLOUR));
  foreground_colour = make_gdk_colour(the_opts.get_l(OPTION_FOREGROUND_COLOUR));
}

#endif


// This function takes the statistics and saves into a selected file
void
Arena_Base::save_statistics_to_file(String filename)
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
          file << "Seq: " << statp->sequence_nr 
               << "  Game: " << statp->game_nr 
               << "  Pos: " << statp->position
               << "  Points: " << statp->points 
               << "  Time Survived: " << statp->time_survived
               << "  Total Points: " << statp->total_points << endl;
        }
    }
}


void
Arena_Base::parse_arena_line(ifstream& file, double& scale, int& succession)
{
  char text[20];
  double radie, bounce_c, hardn, thickness;
  int vertices;

  Vector2D vec1, vec2, vec0;
  WallLine* wall_linep;
  WallCircle* wall_circlep;
  WallInnerCircle* wall_inner_circlep;


  file >> ws;
  file.get(text, 20, ' ');
  if( strcmp(text, "scale" ) == 0 )
    {
      if( succession != 1 ) Error(true, "Error in arenafile: 'scale' not first", 
                                  "Arena_Base::parse_arena_line");
      succession = 2;
      double scl;
      file >> scl;
      scale *= scl;
    }
  else if( strcmp(text, "boundary" ) == 0 )
    {
      if( succession > 2 ) 
        Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate", 
              "Arena_Base::parse_arena_line");
      succession = 3;
      double b1, b2;
      file >> b1;
      file >> b2;
      boundary[0] = Vector2D(scale*b1, scale*b2);
      file >> b1;
      file >> b2;
      boundary[1] = Vector2D(scale*b1, scale*b2);
      if( boundary[1][0] - boundary[0][0] <= 0 || 
          boundary[1][1] - boundary[0][1] <= 0 ) 
        Error(true, "Error in arenafile: 'boundary' negative", 
              "Arena_Base::parse_arena_line");
    }
  else if( strcmp(text, "exclusion_point" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'boundary' after wallpieces or duplicate", 
              "Arena_Base::parse_arena_line");
      file >> vec1;
      Vector2D* excl_p = new Vector2D(scale*vec1);
      g_list_append(exclusion_points, excl_p);
    }
  else if( strcmp(text, "inner_circle" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'inner_circle' before boundary", 
              "Arena_Base::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> vec1;
      file >> radie;
      wall_inner_circlep = new WallInnerCircle(scale*vec1, scale*radie, bounce_c, hardn);
      g_list_insert(object_lists[WALL_INNERCIRCLE_T], wall_inner_circlep, 1);
    }
  else if( strcmp(text, "circle" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'circle' before 'boundary'", 
              "Arena_Base::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> vec1;
      file >> radie;
      wall_circlep = new WallCircle(scale*vec1, scale*radie, bounce_c, hardn);
      g_list_append(object_lists[WALL_CIRCLE_T], wall_circlep);
    }
  //       else if( strcmp(text, "arc" ) == 0 )
  //         {
  //         }
  else if( strcmp(text, "line" ) == 0 )
    {
      if( succession < 3 ) Error(true, "Error in arenafile: 'line' before 'boundary'",
                                 "Arena_Base::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> thickness; 
      thickness *= 0.5;
      file >> vec1;      // start_point
      file >> vec2;      // end_point


      if( length(vec2-vec1) == 0.0 ) 
        Error(true, "Error in arenafile: Zero length line", 
              "Arena_Base::parse_arena_line");

      wall_linep = new WallLine(scale*vec1, unit(vec2-vec1), scale*length(vec2-vec1), 
                                scale*thickness, bounce_c , hardn);      
      g_list_append(object_lists[WALL_LINE_T], wall_linep);
    }
  else if( strcmp(text, "polygon" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'polygon' before 'boundary'", 
              "Arena_Base::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> thickness;
      thickness *= 0.5;
      file >> vertices;   // number of vertices
      file >> vec1;      // first point
      wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
      g_list_append(object_lists[WALL_CIRCLE_T], wall_circlep);

      for(int i=1; i<vertices; i++)
        {
          vec2 = vec1;
          file >> vec1;      // next point

          if( length(vec2-vec1) == 0.0 ) 
            Error(true, "Error in arenafile: Zero length line in polygon", 
                  "Arena_Base::parse_arena_line");

          wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), 
                                    scale*length(vec1-vec2), 
                                    scale*thickness, bounce_c , hardn);      
          g_list_append(object_lists[WALL_LINE_T], wall_linep);
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          g_list_append(object_lists[WALL_CIRCLE_T], wall_circlep);
        }
    }
  else if( strcmp(text, "closed_polygon" ) == 0 )
    {
      if( succession < 3 ) 
        Error(true, "Error in arenafile: 'closed_polygon' before 'boundary'", 
              "Arena_Base::parse_arena_line");
      succession = 4;
      file >> bounce_c;
      file >> hardn;
      file >> thickness;
      thickness *= 0.5;
      file >> vertices;   // number of vertices
      file >> vec1;      // first point
      wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
      g_list_append(object_lists[WALL_CIRCLE_T], wall_circlep);
      vec0 = vec1;

      for(int i=1; i<vertices; i++)
        {
          vec2 = vec1;
          file >> vec1;      // next point

          if( length(vec2-vec1) == 0.0 )
            Error(true, "Error in arenafile: Line in closed_polygon of zero length", 
                  "Arena_Base::parse_arena_line");
          
          wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), 
                                    scale*length(vec1-vec2), 
                                    scale*thickness, bounce_c , hardn);      
          g_list_append(object_lists[WALL_LINE_T], wall_linep);
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          g_list_append(object_lists[WALL_CIRCLE_T], wall_circlep);
        }

      if( length(vec0-vec1) == 0.0 ) 
        Error(true, "Error in arenafile: Last line in closed_polygon of zero length", 
              "Arena_Base::parse_arena_line");

      wall_linep = new WallLine(scale*vec1, unit(vec0-vec1), scale*length(vec0-vec1), 
                                scale*thickness, bounce_c , hardn);      
      g_list_append(object_lists[WALL_LINE_T], wall_linep);
    }
  else if( text[0] != '\0' )
    Error(true, "Incorrect arenafile: unknown keyword" + (String)text, 
          "Arena_Base::parse_arena_line");

}

double
Arena_Base::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, 
                             arenaobject_t& closest_shape, void*& colliding_object, const Robot* the_robot)
{
  double dist = infinity;
  double d;
  closest_shape = NOOBJECT_T;
  GList* gl;

  for(gl = g_list_next(object_lists[ROBOT_T]); gl != NULL; gl = g_list_next(gl))
    {
      if( (Robot*)gl->data != the_robot )
        {
          d = ((Robot*)gl->data)->get_distance(pos, dir, size);
          if( d < dist)
            {
              closest_shape = ROBOT_T;
              colliding_object = gl->data;
              dist = d;
            }
        }
    }
  for(gl = g_list_next(object_lists[WALL_LINE_T]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((WallLine*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = WALL_LINE_T;
          colliding_object = gl->data;
          dist = d;
        }
    }
  for(gl = g_list_next(object_lists[WALL_CIRCLE_T]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((WallCircle*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = WALL_CIRCLE_T;
          colliding_object = gl->data;
          dist = d;
        }
    }
  for(gl = g_list_next(object_lists[WALL_INNERCIRCLE_T]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((WallInnerCircle*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = WALL_INNERCIRCLE_T;
          colliding_object = gl->data;
          dist = d;
        }
    }
  for(gl = g_list_next(object_lists[MINE_T]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Mine*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = MINE_T;
          colliding_object = gl->data;
          dist = d;
        }
    }
  
  for(gl = g_list_next(object_lists[COOKIE_T]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Cookie*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = COOKIE_T;
          colliding_object = gl->data;
          dist = d;
        }
    }

  for(gl = g_list_next(object_lists[SHOT_T]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Shot*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = SHOT_T;
          colliding_object = gl->data;
          dist = d;
        }
    }

  return dist;
}

bool
Arena_Base::space_available(const Vector2D& pos, const double margin)
{
  GList* gl;

  for(gl=g_list_next(object_lists[ROBOT_T]); gl != NULL; gl = g_list_next(gl))
    if( ((Robot*)gl->data)->within_distance(pos, margin) ) return false;


  for(gl=g_list_next(object_lists[WALL_LINE_T]); gl != NULL; gl = g_list_next(gl))
    if( ((WallLine*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[WALL_CIRCLE_T]); gl != NULL; gl = g_list_next(gl))
    if( ((WallCircle*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[WALL_INNERCIRCLE_T]); gl != NULL; gl = g_list_next(gl))
    if( ((WallInnerCircle*)gl->data)->within_distance(pos, margin) ) return false;


  for(gl=g_list_next(object_lists[MINE_T]); gl != NULL; gl = g_list_next(gl))
    if( ((Mine*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[COOKIE_T]); gl != NULL; gl = g_list_next(gl))
    if( ((Cookie*)gl->data)->within_distance(pos, margin) ) return false;

  for(gl=g_list_next(object_lists[SHOT_T]); gl != NULL; gl = g_list_next(gl))
    if( ((Shot*)gl->data)->within_distance(pos, margin) ) return false;

  // Check if it is possible to see any exclusion_points
  
  Vector2D vec;
  double dist;
  arenaobject_t obj_t;
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


double 
Arena_Base::get_shooting_penalty() 
{ 
  return min( the_opts.get_d(OPTION_SHOOTING_PENALTY), 0.5 / (double)robots_left ); 
}


void
Arena_Base::update_timer()
{
  double last_timer = current_timer;

  current_timer = timer.get();
  double timescale = 1.0;
  if( state == GAME_IN_PROGRESS ) timescale = the_opts.get_d(OPTION_TIMESCALE);
  timestep = min( (current_timer - last_timer) * timescale, 
                  the_opts.get_d(OPTION_MAX_TIMESTEP) );
  total_time += timestep;
}

void
Arena_Base::reset_timer()
{
  total_time = 0.0;
  current_timer = 0.0;
  timer.reset();
  update_timer();
}


void
Arena_Base::move_shots()
{
  GList* gl;
  Shot* shotp;

  for(gl = g_list_next(object_lists[SHOT_T]); gl != NULL; )
    {
      shotp = (Shot*)gl->data;

      if( shotp->is_alive() ) shotp->move(timestep);

      gl = g_list_next(gl);
      if( !shotp->is_alive() ) 
        {
          g_list_remove(object_lists[SHOT_T], shotp);
          delete shotp;
        }
    }
}


void 
Arena_Base::set_game_mode( const enum game_mode_t gm)
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
Arena_Base::set_debug_level( const int new_level)
{
  if( new_level > max_debug_level || new_level < 0 || new_level == debug_level ) return debug_level;
  
  debug_level = new_level;

  return debug_level;
}
void
Arena_Base::paus_game_toggle()
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
Arena_Base::step_paused_game()
{
  if( game_mode == DEBUG_MODE && halted )
    {
      halt_next = true; 
      halted = false;
    }
}

bool
Arena_Base::is_game_halted()
{
  return( state == PAUSING_BETWEEN_GAMES || 
          ( game_mode != COMPETITION_MODE && halted ) );
}

void
Arena_Base::delete_lists(const bool kill_robots, const bool del_seq_list, 
                    const bool del_tourn_list, const bool del_arena_filename_list)
{
  GList* gl;
  // clear the lists;
  Robot* robotp;
  for(gl=g_list_next(object_lists[ROBOT_T]); gl != NULL; )
    {
      robotp = (Robot*)gl->data;
      gl=g_list_next(gl);
      g_list_remove(object_lists[ROBOT_T], robotp);
    }
  Shot* shotp;
  for(gl=g_list_next(object_lists[SHOT_T]); gl != NULL; )
    {
      shotp = (Shot*)gl->data;
      delete shotp;
      gl=g_list_next(gl);
      g_list_remove(object_lists[SHOT_T], shotp);
    }
  Mine* minep;
  for(gl=g_list_next(object_lists[MINE_T]); gl != NULL; )
    {
      minep = (Mine*)(gl->data); 
      delete minep;
      gl=g_list_next(gl);
      g_list_remove(object_lists[MINE_T], minep);
    }
  Cookie* cookiep;
  for(gl=g_list_next(object_lists[COOKIE_T]); gl != NULL; )
    {
      cookiep = (Cookie*)(gl->data);
      delete cookiep;
      gl=g_list_next(gl);
      g_list_remove(object_lists[COOKIE_T], cookiep);
    }

  WallLine* walllinep;
  for(gl=g_list_next(object_lists[WALL_LINE_T]); gl != NULL; )
    {
      walllinep = (WallLine*)gl->data;
      delete walllinep;
      gl=g_list_next(gl);
      g_list_remove(object_lists[WALL_LINE_T], walllinep);
    }
  WallCircle* wallcirclep;
  for(gl=g_list_next(object_lists[WALL_CIRCLE_T]); gl != NULL; )
    {
      wallcirclep = (WallCircle*)gl->data;
      delete wallcirclep;
      gl=g_list_next(gl);
      g_list_remove(object_lists[WALL_CIRCLE_T], wallcirclep);
    }
  WallInnerCircle* wallinnercirclep;
  for(gl=g_list_next(object_lists[WALL_INNERCIRCLE_T]); gl != NULL; )
    {
      wallinnercirclep = (WallInnerCircle*)gl->data;
      delete wallinnercirclep;
      gl=g_list_next(gl);
      g_list_remove(object_lists[WALL_INNERCIRCLE_T], wallinnercirclep);
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
