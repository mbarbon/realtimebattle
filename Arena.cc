#include <math.h>
#include <signal.h>
#include <unistd.h>
#include "Arena.h"
#include "gui.h"

Arena::Arena()
{
  state = NOT_STARTED;
  timer = g_timer_new();
  timescale = 1.0;
  
  all_robots_in_sequence = g_list_alloc();
  all_robots_in_tournament = g_list_alloc();
  arena_filenames = g_list_alloc();
  exclusion_points = g_list_alloc();

  background_colour = make_gdk_color(opts.get_background_colour());
  foreground_colour = make_gdk_color(opts.get_foreground_colour());

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
  g_list_free(exclusion_points);
  for(int i=ROBOT; i<EXPLOSION; i++)
    g_list_free(object_lists[i]);
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
  double scale = 1.0;
  do
    {
      file >> ws >> text;
      if( strcmp(text, "scale" ) == 0 )
        {
          if( succession != 1 ) throw Error("'scale' not first", "Arena::parsefile");
          succession = 2;
          file >> scale;
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

              wall_linep = new WallLine(scale*vec2, unit(vec1-vec2), scale*length(vec1-vec2), 
                                        scale*thickness, bounce_c , hardn);      
              g_list_append(object_lists[WALL], wall_linep);
              wall_circlep = new WallCircle(scale*vec1, scale*thickness, bounce_c, hardn);
              g_list_append(object_lists[WALL], wall_circlep);
            }
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

        // Place mines and cookies
        if( ((double)rand()) / (double)RAND_MAX <= timestep*opts.get_cookie_frequency() )
          add_cookie();

        if( ((double)rand()) / (double)RAND_MAX <= timestep*opts.get_mine_frequency() )
          add_mine();
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
  gdouble last_timer = current_timer;
  gulong microsecs;
  current_timer = g_timer_elapsed(timer, &microsecs);
  timestep = min( (current_timer - last_timer) * timescale, opts.get_max_timestep() );
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
  double en = opts.get_cookie_min_energy() + 
    (opts.get_cookie_max_energy() - opts.get_cookie_min_energy()) * 
    (double)rand() / (double)RAND_MAX;
  bool found_space = false;
  double r = opts.get_cookie_radius();
  Vector2D pos;

  for( int i=0; i<100 && !found_space; i++)
    {
      pos = get_random_position();
      found_space = space_available(pos, r*2.0);
    }
  
  if( !found_space ) throw Error("Couldn't find space for cookie", "Arena::timeout_function");
  Cookie* cookiep = new Cookie(pos, r, en, this);
  g_list_append(object_lists[COOKIE], cookiep);
}

void
Arena::add_mine()
{
  double en = opts.get_mine_min_energy() + 
    (opts.get_mine_max_energy() - opts.get_mine_min_energy()) * 
    (double)rand() / (double)RAND_MAX;
  bool found_space = false;
  double r = opts.get_cookie_radius();
  Vector2D pos;

  for( int i=0; i<100 && !found_space; i++)
    {
      pos = get_random_position();
      found_space = space_available(pos, r*2.0);
    }
  
  if( !found_space ) throw Error("Couldn't find space for mine", "Arena::timeout_function");
  Mine* minep = new Mine(pos, r, en, this);
  g_list_append(object_lists[MINE], minep);
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

GdkColor
make_gdk_color(const long col)
{
  GdkColormap *cmap;
  GdkColor colour;

  cmap = gdk_colormap_get_system();
  colour.red =   ((col & 0xff0000) >> 16 ) * 0x101;
  colour.green = ((col & 0x00ff00) >> 8  ) * 0x101;
  colour.blue =  (col & 0x0000ff) * 0x101;
  if( !gdk_color_alloc (cmap, &colour) )
    throw Error("Couldn't allocate colour", "Arena::make_gdk_color");

  return colour;
}

double
Arena::colour_dist(const long col1, const GdkColor& col2)
{
  return( abs((col1 & 0xff)*0x101 - col2.blue)*
          log(1.0 + sqrt((col1 & 0xff)*0x101 + col2.blue))/log(2.0) +
          abs(((col1 & 0xff00) >> 8)*0x101 - col2.green)*
          log(1.0 + sqrt(((col1 & 0xff00) >> 8)*0x101 + col2.green))/log(2.0) +
          abs(((col1 & 0xff0000) >> 16)*0x101 - col2.red)*
          log(1.0 + sqrt(((col1 & 0xff0000) >> 16)*0x101 + col2.red))/log(2.0));
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
  
  if( colour_dist( colour, background_colour ) < min_dist ) return false;

  return true;
}

long
Arena::find_free_color(const long home_colour, const long away_colour, const Robot* robotp)
{  
  long tmp_colour;

  for(double min_dist = 130000.0; min_dist > 0.5 ; min_dist *= 0.8)
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
  Vector2D* vecp;
  for(gl=g_list_next(exclusion_points); gl != NULL; )
    {
      vecp = (Vector2D*)gl->data;
      gl=g_list_next(gl);
      g_list_remove(exclusion_points, vecp);
    }
  if( del_seq_list )
    for(gl=g_list_next(all_robots_in_sequence); gl != NULL; )
      {
        robotp = (Robot*)gl->data;
        gl=g_list_next(gl);
        g_list_remove(all_robots_in_sequence, robotp);
      }
  if( del_tourn_list )
    for(gl=g_list_next(all_robots_in_tournament); gl != NULL; )
      {
        robotp = (Robot*)gl->data;
        gl=g_list_next(gl);
        if( kill_robots ) delete robotp;
        g_list_remove(all_robots_in_tournament, robotp);
      }
}
void
Arena::start_game()
{
  // put the arena together
  
  current_arena_nr = current_arena_nr % number_of_arenas + 1;
  
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

  robots_left = 0;
  for( ; gl != NULL; gl=g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      found_space = false;
      for( int i=0; i<100 && !found_space; i++)
        {
          pos = get_random_position();
          found_space = space_available(pos, opts.get_robot_radius()*1.2);
        }

      if( !found_space )
        throw Error("Couldn't find space for all robots", "Arena::start_game");
      angle = ((double)rand())*2.0*M_PI/RAND_MAX;
      robotp->set_initial_values(pos, angle);
      g_list_append(object_lists[ROBOT], gl->data);
      robots_left++;
      ((Robot*)gl->data)->live();
    }

  broadcast(GAME_STARTS);
  
  state = GAME_IN_PROGRESS;
  games_remaining_in_sequence--;

  the_gui->setup_arena_window( boundary );
  the_gui->setup_score_window();

  reset_timer();
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
  current_arena_nr = 0;

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
  number_of_arenas = 0;

  for(int i=0; arenafilename_list[i] != NULL; i++)
    {
      // TODO: check if arena filename is ok
      g_list_append(arena_filenames, g_string_new(arenafilename_list[i]));
      number_of_arenas++;
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
