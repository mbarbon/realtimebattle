#include "Arena.h"

Arena::Arena()
{
  state = NOT_STARTED;
  g_timer_start(&timer);
}

Arena::~Arena()
{
  //  if( filep ) delete filep;
}

void
Arena::parse_file(istream& file)
{
  char text[20];
  double number1, number2;

  Vector2D vec1, vec2;
  Wall* wallp;

  do
    {
      file >> ws >> text;
      if( strcmp(text, "boundary" ) == 0 )
        {
          file >> boundary[1];
          file >> boundary[2];
          file >> boundary[3];
          file >> boundary[4];
        }
      else if( strcmp(text, "circle" ) == 0 )
        {
          file >> ws >> vec1;
          file >> ws >> number1;

          wallp = new WallCircle(vec1, number1);
          g_list_append(&object_lists[WALL], wallp);
          g_list_append(&solid_objects, wallp);
        }
      //  else if( strcmp(text, "outer_circle" ) == 0 )
      //    {
      //    }
      //   else if( strcmp(text, "arc" ) == 0 )
      //     {
      //     }
      else if( strcmp(text, "line" ) == 0 )
        {
          file >> ws >> vec1;      // start_point
          file >> ws >> vec2;      // end_point
          file >> ws >> number2;   // thickness
          
          wallp = new WallLine(vec1, unit(vec2-vec1), length(vec2-vec1), number2);      
          g_list_append(&object_lists[WALL], wallp);
          g_list_append(&solid_objects, wallp);
        }
      //   else if( strcmp(text, "polygon" ) == 0 )
      //     {
      //     }
      //   else if( strcmp(text, "closed_polygon" ) == 0 )
      //     {
      //     }
      else
        throw Error("Incorrect arenafile, unknown keyword", 
                    text, "Arena::parsefile");
        
    } while( text[0] != '\0' );
}

void
Arena::add_to_list(class ArenaObject& obj)
{
  g_list_append(&object_lists[obj.get_object_type()], &obj);
}  

void
Arena::add_to_solid_object_list(class Shape& obj)
{
  g_list_append(&solid_objects, &obj);
}
  
void
Arena::remove_from_list(class ArenaObject& obj)
{
  g_list_remove(&object_lists[obj.get_object_type()], &obj);
}  

void
Arena::remove_from_solid_object_list(class Shape& obj)
{
  g_list_remove(&solid_objects, &obj);
}  


double
Arena::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, Shape* closest_shape)
{
  double dist = infinity;
  double d;
  Shape* objp;
  closest_shape = NULL;
  GList* gl;

  for(gl = g_list_first(&solid_objects); gl->next != NULL; gl = g_list_next(gl))
    {
      objp = (Shape*)gl->data;
      objp->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = objp;
          dist = d;
        }
    }

  return dist;
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
      if( total_time > next_check_time ) start_sequence_follow_up();
      break;
      
    case SHUTTING_DOWN_ROBOTS:
      if( total_time > next_check_time ) end_sequence_follow_up();
      break;
      
    case GAME_IN_PROGRESS:
      update();
      if( robots_left <= 1 ) end_game();
      //      if( total_time > next_check_time ) check_robots();
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
  //move_shots();
  update_robots();
}

void
Arena::update_timer()
{
  gdouble last_time = total_time;
  gulong microsecs;
  total_time = g_timer_elapsed(&timer, &microsecs);
  timestep = total_time - last_time;
}

void
Arena::update_robots()
{
  GList* gl;
  Robot* robotp;

  for(gl = g_list_first(&object_lists[ROBOT]); gl->next != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      if( robotp->is_alive() )
        {
          robotp->move(timestep);
          if( !robotp->is_alive() ) remove_from_solid_object_list(*(Shape*)robotp);
          
          robotp->update_radar_and_cannon(timestep);  
        }      
    }
}

void
Arena::start_game()
{
  // put the arena together
  
  GString* filename = (GString*)g_list_nth(&arena_filenames, current_arena_nr)->data;
  ifstream file(filename->str);
  if( !file ) throw Error("Couldn't open file", filename->str, "Arena::Arena");

  parse_file(file);

  // Place robots on the arena


  // Make list of living robots and tell them to get ready

  GList* gl = g_list_first(&all_robots_in_sequence);
  robots_left = 0;
  for( ; gl != NULL; gl=g_list_next(gl))
    {
      g_list_append(&object_lists[ROBOT], gl->data);
      g_list_append(&solid_objects, gl->data);
      ((Robot*)gl->data)->send_message(GAME_STARTS);
      robots_left++;
    }

  games_remaining_in_sequence--;
  g_timer_reset(&timer);
  update_timer();
}

void
Arena::end_game()
{
  // pull down the arena
  GList* gl = g_list_first(&object_lists[WALL]);

  for(; gl != NULL; gl = g_list_next(gl))
    {
      delete gl->data;
    }

  // delete some lists;
  for(int i= ROBOT; i < EXPLOSION; i++)
    g_list_free(&object_lists[i]);
 
  g_list_free(&solid_objects);
 
  if(games_remaining_in_sequence == 0) end_sequence();
}


void
Arena::start_sequence()
{
  games_remaining_in_sequence = games_per_sequence;

  // Make list of robots in this sequence

  GList* gl = g_list_first(&all_robots_in_tournament);

  for(int i=0; i<robots_per_game; i++)
    {
      g_list_append(&all_robots_in_sequence, gl->data);
      gl = g_list_next(gl);
    }

  // execute robot processes
  gl = g_list_first(&all_robots_in_sequence);

  for(; gl != NULL; gl = g_list_next(gl))
    {
      ((Robot*)gl->data)->start_process();
    }
  
  // wait a second before checking
  
  next_check_time = total_time + 1.0;
}

void
Arena::start_sequence_follow_up()
{
  // check if the process have started correctly
  GList* gl = g_list_first(&all_robots_in_sequence);
  Robot* robotp;

  for(; gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      if( !(robotp->is_process_running()) ) 
        {
          g_list_remove(&all_robots_in_sequence, robotp);
          robots_left--;
        }
    }
  start_game();
}

void
Arena::end_sequence()
{
  // kill all robot processes
  
  GList* gl = g_list_first(&all_robots_in_sequence);

  for(; gl != NULL; gl = g_list_next(gl))
    {
      ((Robot*)gl->data)->send_message(EXIT_ROBOT);
    }

  // wait a second before checking
  
  next_check_time = total_time + 1.0;
}

void
Arena::end_sequence_follow_up()
{
  // check if the process have stopped, otherwise kill
  
  GList* gl = g_list_first(&all_robots_in_sequence);
  Robot* robotp;

  for(; gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      if( robotp->is_process_running() ) robotp->kill_process_forcefully();
    }

  if(sequences_remaining == 0) 
    end_tournament();
  else
    start_sequence();
}

void
Arena::start_tournament(char** robotfilename_list, char** arenafilename_list, int robots_p_game, int games_p_sequence)
{
  // Create robot classes and to into the list all_robots_in_tournament

  Robot* robotp;

  for(int i=0; robotfilename_list[i] != NULL; i++)
    {
      robotp = new Robot(robotfilename_list[i]);
      g_list_append(&all_robots_in_tournament, robotp);
    }

  // Create list of arena filenames

  for(int i=0; arenafilename_list[i] != NULL; i++)
    {
      g_list_append(&arena_filenames, g_string_new(arenafilename_list[i]));
    }

  robots_per_game = robots_p_game;
  games_per_sequence = games_p_sequence;
  start_sequence();
    
}

void
Arena::end_tournament()
{
  // delete all robot classes in all_robots_in_tournament

  GList* gl = g_list_first(&all_robots_in_tournament);

  for(; gl != NULL; gl = g_list_next(gl))
    {
      delete gl->data;
    }
  g_list_free(&all_robots_in_tournament);
}







