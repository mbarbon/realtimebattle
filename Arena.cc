#include <math.h>
#include <signal.h>
#include <unistd.h>

#include "Arena.h"
#include "MovingObject.h"
#include "Shape.h"
#include "Extras.h"
#include "Error.h"
#include "gui.h"
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
      the_gui.close_score_window();
      the_gui.close_arena_window();
      the_gui.close_message_window();

      delete_lists(true, true, false, true);
      
      state = FINISHED;
    }
}

void
Arena::set_colours()
{  
  background_colour = make_gdk_colour(the_opts.get_l(OPTION_BACKGROUND_COLOUR));
  foreground_colour = make_gdk_colour(the_opts.get_l(OPTION_FOREGROUND_COLOUR));
}

void
Arena::parse_file(istream& file)
{
  char text[20];
  double number1, number2, bounce_c, hardn, thickness;
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
          if( succession != 1 ) throw Error("'scale' not first", "Arena::parsefile");
          succession = 2;
          double scl;
          file >> scl;
          scale *= scl;
        }
      else if( strcmp(text, "boundary" ) == 0 )
        {
          if( succession > 2 ) throw Error("'boundary' after wallpieces or duplicate", "Arena::parsefile");
          succession = 3;
          double b1, b2;
          file >> b1;
          file >> b2;
          boundary[0] = Vector2D(scale*b1, scale*b2);
          file >> b1;
          file >> b2;
          boundary[1] = Vector2D(scale*b1, scale*b2);
        }
      else if( strcmp(text, "exclusion_point" ) == 0 )
        {
          if( succession < 3 ) throw Error("'boundary' after wallpieces or duplicate", "Arena::parsefile");
          file >> vec1;
          Vector2D* excl_p = new Vector2D(scale*vec1);
          g_list_append(exclusion_points, excl_p);
        }
      else if( strcmp(text, "inner_circle" ) == 0 )
        {
          if( succession != 3 ) throw Error("'inner_circle' not between boundary other wallpieces", "Arena::parsefile");
          file >> vec1;
          file >> number1;
          file >> bounce_c;
          file >> hardn;
          wall_inner_circlep = new WallInnerCircle(scale*vec1, scale*number1, bounce_c, hardn);
          g_list_append(object_lists[WALL], wall_inner_circlep);
        }
      else if( strcmp(text, "circle" ) == 0 )
        {
          if( succession < 3 ) throw Error("'circle' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> vec1;
          file >> number1;
          file >> bounce_c;
          file >> hardn;

          wall_circlep = new WallCircle(scale*vec1, scale*number1, bounce_c, hardn);
          g_list_append(object_lists[WALL], wall_circlep);
        }
//       else if( strcmp(text, "arc" ) == 0 )
//         {
//         }
      else if( strcmp(text, "line" ) == 0 )
        {
          if( succession < 3 ) throw Error("'line' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> vec1;      // start_point
          file >> vec2;      // end_point
          file >> number2;   // thickness
          file >> bounce_c;
          file >> hardn;

          if( length(vec2-vec1) == 0.0 ) throw Error("Zero length line", "Arena::parsefile");

          wall_linep = new WallLine(scale*vec1, unit(vec2-vec1), scale*length(vec2-vec1), 
                                    scale*number2, bounce_c , hardn);      
          g_list_append(object_lists[WALL], wall_linep);
        }
      else if( strcmp(text, "polygon" ) == 0 )
        {
          if( succession < 3 ) throw Error("'polygon' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> thickness;    // thickness
          file >> bounce_c;
          file >> hardn;
          file >> vertices;   // number of vertices
          file >> vec1;      // first point
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          g_list_append(object_lists[WALL], wall_circlep);

          for(int i=1; i<vertices; i++)
            {
              vec2 = vec1;
              file >> vec1;      // next point

              if( length(vec2-vec1) == 0.0 ) throw Error("Zero length line in polygon", "Arena::parsefile");

              wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), scale*length(vec1-vec2), 
                                        scale*thickness, bounce_c , hardn);      
              g_list_append(object_lists[WALL], wall_linep);
              wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
              g_list_append(object_lists[WALL], wall_circlep);
            }
        }
      else if( strcmp(text, "closed_polygon" ) == 0 )
        {
          if( succession < 3 ) throw Error("'closed_polygon' before 'boundary'", "Arena::parsefile");
          succession = 4;
          file >> thickness;    // thickness
          file >> bounce_c;
          file >> hardn;
          file >> vertices;   // number of vertices
          file >> vec1;      // first point
          wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
          g_list_append(object_lists[WALL], wall_circlep);
          vec0 = vec1;

          for(int i=1; i<vertices; i++)
            {
              vec2 = vec1;
              file >> vec1;      // next point

              if( length(vec2-vec1) == 0.0 ) throw Error("Line in closed_polygon of zero length", "Arena::parsefile");
          
              wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), scale*length(vec1-vec2), 
                                        scale*thickness, bounce_c , hardn);      
              g_list_append(object_lists[WALL], wall_linep);
              wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
              g_list_append(object_lists[WALL], wall_circlep);
            }

          if( length(vec0-vec1) == 0.0 ) throw Error("Last line in closed_polygon of zero length", "Arena::parsefile");
          wall_linep = new WallLine(scale*vec1, unit(vec0-vec1), scale*length(vec0-vec1), 
                                    scale*thickness, bounce_c , hardn);      
          g_list_append(object_lists[WALL], wall_linep);
        }
      else if( text[0] != '\0' )
        throw Error("Incorrect arenafile, unknown keyword", 
                    text, "Arena::parsefile");
        
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
  return the_opts.get_d(OPTION_SHOOTING_PENALTY) / max(1.0, ((double)robots_left)/10.0); 
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
          throw Error("Couldn't send message, no arg_type", "Robot::send_message");
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
          throw Error("Couldn't send message, unknown arg_type", "Robot::send_message");
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
  int old_total = (int)total_time;
  update_timer ();

  switch(state)
    {
    case NOT_STARTED:
    case FINISHED:
      break;
      
    case STARTING_ROBOTS:
      {
        GList* gl;
        
        for(gl = g_list_next(all_robots_in_sequence); gl != NULL; gl = g_list_next(gl) )
          ((Robot*)gl->data)->get_messages();

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
        if((int)total_time > old_total) the_gui.set_score_window_title();
        if( robots_left <= 1 || total_time > the_opts.get_d(OPTION_TIMEOUT) ) 
          {
            for(GList* gl=g_list_next(all_robots_in_sequence); gl != NULL; gl=g_list_next(gl))
              if( ((Robot*)gl->data)->get_position_this_game() == 0 )
                {
                  ((Robot*)gl->data)->die();
                  ((Robot*)gl->data)->set_stats(1);
                }
            end_game();
          }
        else
          {
            if( COMPETITION_MODE && total_time > next_check_time ) check_robots();
            
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
      throw Error("Arena state is NO_STATE, shouldn't ever happen!", "Arena::timeout_function");
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
  //update_explosions();
  move_shots();
  update_robots();
  the_gui.draw_objects();
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
  
  if( !found_space ) throw Error("Couldn't find space for cookie", "Arena::timeout_function");
  Cookie* cookiep = new Cookie(pos, r, en);
  g_list_append(object_lists[COOKIE], cookiep);
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
  
  if( !found_space ) throw Error("Couldn't find space for mine", "Arena::timeout_function");
  Mine* minep = new Mine(pos, r, en);
  g_list_append(object_lists[MINE], minep);
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
      if( robotp->is_alive() ) robotp->get_messages();
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
        robotp->send_message( ENERGY, (int)( robotp->get_energy() / (double)the_opts.get_l(OPTION_ROBOT_ENERGY_LEVELS) ) );
    }

  if( killed_robots > 0 )
    {
      for( gl=g_list_next(object_lists[ROBOT]); gl != NULL; gl=g_list_next(gl))
        {
          ((Robot*)gl->data)->add_points(killed_robots);
          if( robots_left < 15 ) ((Robot*)gl->data)->display_score();
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
   throw Error("Impossible to find colour", "Arena::find_free_colour");
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
  if( !file ) throw Error("Couldn't open file", *filename, "Arena::Arena");

  parse_file(file);

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

      if( !found_space )
        throw Error("Couldn't find space for all robots", "Arena::start_game");
      angle = ((double)rand())*2.0*M_PI/RAND_MAX;
      robotp->set_values_before_game(pos, angle);
      g_list_append(object_lists[ROBOT], gl->data);
      robots_left++;
      ((Robot*)gl->data)->live();
    }

  broadcast(GAME_STARTS);
  broadcast(ROBOTS_LEFT, robots_left);
  the_opts.broadcast_opts();

  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl=g_list_next(gl))
    ((Robot*)gl->data)->send_signal();

  state = GAME_IN_PROGRESS;
  games_remaining_in_sequence--;

  reset_timer();  // Time should be zero in score window

  the_gui.setup_arena_window( boundary );
  the_gui.setup_score_window();

  reset_timer();  // Game starts !
  next_check_time = total_time + the_opts.get_d(OPTION_CHECK_INTERVAL);
}

void
Arena::end_game()
{
  // Close Score, Message and Control Windows

  the_gui.close_score_window();
  the_gui.close_arena_window();

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
      try
        {
          ((Robot*)gl->data)->start_process();
        }
      catch ( Error the_error )
        {
          the_error.print_message();
          exit( EXIT_FAILURE );
        }

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

  the_gui.setup_message_window();

  number_of_robots = 0;
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

          if( k < 0 ) throw Error("Problem generating list of participants, k < 0", 
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
      for(int i=0; i<number_of_robots; i++) robot_playing_this_match[i] = false;

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

  sequence_nr = 0;
  start_sequence();
}

void
Arena::end_tournament()
{
  state = FINISHED;

  the_gui.close_message_window();
  //  the_gui.close_control_window();
}
