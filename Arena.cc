#include <math.h>
#include <signal.h>
#include <unistd.h>
#include "Arena.h"
#include "gui.h"

Arena::Arena()
{
  state = NOT_STARTED;
  timer = g_timer_new();
  all_robots_in_sequence = g_list_alloc();
  all_robots_in_tournament = g_list_alloc();
  arena_filenames = g_list_alloc();
  robot_radius = 0.5;
  robot_mass = 1.0;
  robot_hardness = 0.5;
  robot_protection = 0.5;
  robot_bounce_coeff = 0.7;
  shot_radius = 0.1;
  shot_speed = 10.0;
  max_acceleration = 2.0;
  min_acceleration = -0.5;
  start_energy = 100.0;
  shooting_penalty = 0.075;
  air_resistance = 0.005;
  roll_friction = 0.002;
  slide_friction = 0.098;
  grav_const = 9.82;
  for(int i=ROBOT; i<=EXPLOSION; i++)
    object_lists[i] = g_list_alloc();
  the_gui = new Gui(this);
}

Arena::~Arena()
{
  //  if( filep ) delete filep;
  state=EXITING;
  sleep(1);
  g_timer_destroy(timer);
  delete_lists(true, true, true);
  g_list_free(all_robots_in_sequence);
  g_list_free(all_robots_in_tournament);
  g_list_free(arena_filenames);
  for(int i=ROBOT; i<EXPLOSION; i++)
    g_list_free(object_lists[i]);
}

void
Arena::parse_file(istream& file)
{
  char text[20];
  double number1, number2, bounce_c, hardn;

  Vector2D vec1, vec2;
  //Wall* wallp;

  do
    {
      file >> ws >> text;
      if( strcmp(text, "boundary" ) == 0 )
        {
          double b1, b2;
          file >> b1;
          file >> b2;
          boundary[0] = Vector2D(b1, b2);
          file >> b1;
          file >> b2;
          boundary[1] = Vector2D(b1, b2);
        }
      else if( strcmp(text, "circle" ) == 0 )
        {
          file >> vec1;
          file >> number1;
          file >> bounce_c;
          file >> hardn;

          WallCircle* wallp = new WallCircle(vec1, number1, bounce_c, hardn);
          g_list_append(object_lists[WALL], wallp);
        }
      //  else if( strcmp(text, "outer_circle" ) == 0 )
      //    {
      //    }
      //   else if( strcmp(text, "arc" ) == 0 )
      //     {
      //     }
      else if( strcmp(text, "line" ) == 0 )
        {
          file >> vec1;      // start_point
          file >> vec2;      // end_point
          file >> number2;   // thickness
          file >> bounce_c;
          file >> hardn;

          WallLine* wallp = new WallLine(vec1, unit(vec2-vec1), length(vec2-vec1), 
                                         number2, bounce_c , hardn);      
          g_list_append(object_lists[WALL], wallp);
        }
      //   else if( strcmp(text, "polygon" ) == 0 )
      //     {
      //     }
      //   else if( strcmp(text, "closed_polygon" ) == 0 )
      //     {
      //     }
      else if( text[0] != '\0' )
        throw Error("Incorrect arenafile, unknown keyword", 
                    text, "Arena::parsefile");
        
    } while( text[0] != '\0' );
}

double
Arena::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, 
                             object_type& closest_shape, void*& colliding_object)
{
  double dist = infinity;
  double d;
  closest_shape = NOOBJECT;
  GList* gl;

  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    {
      d = ((Robot*)gl->data)->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = ROBOT;
          colliding_object = gl->data;
          dist = d;
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

void
Arena::broadcast(const message_to_robot_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);
  GList* gl;
  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; gl = g_list_next(gl))
    ((Robot*)gl->data)->send_message(msg_type, args);
}

void
Arena::quit_ordered()
{
  state = EXITING;
}

gint
Arena::timeout_function()
{
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
        if( robots_left <= 1 ) 
          {
            for(GList* gl=g_list_next(all_robots_in_sequence); gl != NULL; gl=g_list_next(gl))
              if( ((Robot*)gl->data)->get_position_this_game() == 0 )
                {
                  ((Robot*)gl->data)->die();
                  ((Robot*)gl->data)->set_stats(1);
                }
            end_game();
          }
      //   TODO:    if( total_time > next_check_time ) check_robots();
      }
      break;

    case EXITING:
      return false;
    }

  return true;
}

void
Arena::update()
{
  //update_explosions();
  move_shots();
  update_robots();
  the_gui->draw_objects();
}

void
Arena::update_timer()
{
  gdouble last_time = total_time;
  gulong microsecs;
  total_time = g_timer_elapsed(timer, &microsecs);
  timestep = total_time - last_time;
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
  for(gl = g_list_next(object_lists[ROBOT]); gl != NULL; )
    {
      robotp = (Robot*)gl->data;
      if( robotp->is_alive() )
        {
          robotp->update_radar_and_cannon(timestep);  
          robotp->change_velocity(timestep);
          robotp->move(timestep);        
        }
      if( robotp->is_alive() ) robotp->get_messages();

      gl = g_list_next(gl);
      if( !robotp->is_alive() ) 
        {
          g_list_remove(object_lists[ROBOT], robotp);          
          killed_robots++;
        }
    }

  if( killed_robots > 0 )
    {
      for( gl=g_list_next(all_robots_in_sequence); gl != NULL; gl=g_list_next(gl))
          if( ((Robot*)gl->data)->get_position_this_game() == robots_left )
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
  return( abs((col1 & 0xff)*0x101 - col2.blue) +
          abs(((col1 & 0xff00) >> 8)*0x101 - col2.green) +
          abs(((col1 & 0xff0000) >> 16)*0x101 - col2.red) );
}

bool
Arena::is_colour_allowed(const long colour, const double min_dist, const Robot* robotp)
{
  for(GList* gl = g_list_next(all_robots_in_sequence); gl != NULL; gl = g_list_next(gl))
    {
      if((Robot *)gl->data != robotp)
        {
          if( colour_dist( colour, ((Robot*)gl->data)->get_colour() ) < min_dist ) return false;
        }
    }
  // TODO: check background color
  return true;
}

long
Arena::find_free_color(const long home_colour, const long away_colour, const Robot* robotp)
{  
  long tmp_colour;

  for(double min_dist = 50.0; min_dist > 0.5 ; min_dist *= 0.8)
    {
      if( is_colour_allowed(home_colour, min_dist, robotp) ) return home_colour;
      if( is_colour_allowed(away_colour, min_dist, robotp) ) return away_colour;
      for(int i=0; i<25; i++)
        {
          tmp_colour = rand() & 0xffffff;
          if( is_colour_allowed(tmp_colour, min_dist, robotp) ) return tmp_colour;
        }                  
    }
   throw Error("Impossible to find colour", "Arena::find_free_colour");
}

void
Arena::delete_lists(bool kill_robots, bool del_seq_list, bool del_tourn_list)
{
  GList* gl;
  // clear the lists;
  Robot* robotp;
  for(gl=g_list_next(object_lists[ROBOT]); gl != NULL; )
    {
      robotp = (Robot*)gl->data;
      if( kill_robots ) delete robotp;
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
  Explosion* explosionp;
  for(gl=g_list_next(object_lists[EXPLOSION]); gl != NULL; )
    {
      explosionp = (Explosion*)(gl->data);
      delete explosionp;
      gl=g_list_next(gl);
      g_list_remove(object_lists[EXPLOSION], explosionp);
    }
  if( del_seq_list )
    for(gl=g_list_next(all_robots_in_sequence); gl != NULL; gl=g_list_next(gl))
      g_list_remove(all_robots_in_sequence, (Robot*)gl->data);
  if( del_tourn_list )
    for(gl=g_list_next(all_robots_in_tournament); gl != NULL; gl=g_list_next(gl))
      g_list_remove(all_robots_in_tournament, (Robot*)gl->data);
}
void
Arena::start_game()
{
  // put the arena together
  
  GString* filename = (GString*)g_list_nth(arena_filenames, current_arena_nr)->data;
  ifstream file(filename->str);
  if( !file ) throw Error("Couldn't open file", filename->str, "Arena::Arena");

  parse_file(file);

  // Place robots on the arena

  GList* gl = g_list_next(all_robots_in_sequence);
  Robot* robotp;
  bool found_space;
  double angle;
  Vector2D pos;

  for( ; gl != NULL; gl=g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      found_space = false;
      for( int i=0; i<100 && !found_space; i++)
        {
          pos = get_random_position();
          found_space = space_available(pos, robot_radius*1.2);
        }

      if( !found_space )
        throw Error("Couldn't find space for all robots", "Arena::start_game");
      angle = ((double)rand())*2.0*M_PI/RAND_MAX;
      robotp->set_initial_values(pos, angle);
    }

  // Make list of living robots and tell them to get ready

  gl = g_list_next(all_robots_in_sequence);
  robots_left = 0;
  for( ; gl != NULL; gl=g_list_next(gl))
    {
      g_list_append(object_lists[ROBOT], gl->data);
      ((Robot*)gl->data)->send_message(GAME_STARTS);
      robots_left++;
      ((Robot*)gl->data)->live();
    }

  state = GAME_IN_PROGRESS;
  games_remaining_in_sequence--;

  the_gui->setup_arena_window( boundary );
  the_gui->setup_score_window();

  g_timer_reset(timer);
  update_timer();
}

void
Arena::end_game()
{
  // Close Score, Message and Control Windows

  the_gui->close_score_window();
  the_gui->close_arena_window();

  broadcast(GAME_FINISHES);

  delete_lists(false, false, false);
  
  if(games_remaining_in_sequence == 0) 
    end_sequence();
  else
    start_game();
}


void
Arena::start_sequence()
{
  games_remaining_in_sequence = games_per_sequence;
  current_arena_nr = 1;

  // Make list of robots in this sequence

  GList* gl = g_list_next(all_robots_in_tournament);

  for(int i=0; i<robots_per_game; i++)
    {
      g_list_append(all_robots_in_sequence, gl->data);
      gl = g_list_next(gl);
    }

  // execute robot processes
  gl = g_list_next(all_robots_in_sequence);

  for(; gl != NULL; gl = g_list_next(gl))
    {
      ((Robot*)gl->data)->start_process();
    }
  
  // wait a second before checking

  state = STARTING_ROBOTS;
  sequence_nr++;
  sequences_remaining--;
  g_timer_reset(timer);
  update_timer();
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
      g_list_remove(all_robots_in_sequence, robotp);
    }

  if(sequences_remaining == 0) 
    end_tournament();
  else
    start_sequence();
}

void
Arena::start_tournament(char** robotfilename_list, char** arenafilename_list, int robots_p_game, int games_p_sequence, int n_o_sequences)
{
  // Create robot classes and to into the list all_robots_in_tournament

  //  the_gui->setup_control_window();
  the_gui->setup_message_window();

  Robot* robotp;

  for(int i=0; robotfilename_list[i] != NULL; i++)
    {
      robotp = new Robot(robotfilename_list[i], this);
      g_list_append(all_robots_in_tournament, robotp);
    }

  // Create list of arena filenames

  for(int i=0; arenafilename_list[i] != NULL; i++)
    {
      g_list_append(arena_filenames, g_string_new(arenafilename_list[i]));
    }

  robots_per_game = robots_p_game;
  games_per_sequence = games_p_sequence;
  sequences_remaining = n_o_sequences;
  sequence_nr = 0;
  start_sequence();
}

void
Arena::end_tournament()
{
  // delete all robot classes in all_robots_in_tournament

  //  GList* gl = g_list_next(all_robots_in_tournament);

  //  for(; gl != NULL; gl = g_list_next(gl))
  //    {
  //      delete gl->data;
  //    }

  state = FINISHED;

  the_gui->close_message_window();
  //  the_gui->close_control_window();
}
