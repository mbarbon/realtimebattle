#include "Arena.h"

Arena::Arena()
{

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
  Shape* shapep;

  do
    {
      file >> ws >> text;
      
      if( strcmp(text, "circle" ) == 0 )
        {
          file >> ws >> vec1;
          file >> ws >> number1;

          shapep = new Circle(vec1, number1);
          g_list_append(&object_lists[WALL], shapep);
          g_list_append(&solid_objects, shapep);
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
          
          shapep = new Line(vec1, unit(vec2-vec1), length(vec2-vec1), number2);      
          g_list_append(&object_lists[WALL], shapep);
          g_list_append(&solid_objects, shapep);
        }
      //   else if( strcmp(text, "polygon" ) == 0 )
      //     {
      //     }
      //   else if( strcmp(text, "closed_polygon" ) == 0 )
      //     {
      //     }
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
Arena::update()
{
  //update_explosions();
  //move_shots();
  update_robots();

  return true;
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
Arena::start_game(int arenanr)
{
  
  GString* filename = (GString*)g_list_nth(&arena_filenames, arenanr)->data;
  ifstream file(filename->str);
  if( !file ) throw Error("Couldn't open file", filename->str, "Arena::Arena");

  parse_file(file);

  GList* gl = g_list_first(&all_robots_in_sequence);
  for( ; gl != NULL; gl=g_list_next(gl))
    {
      g_list_append(&object_lists[ROBOT], gl->data);
      ((Robot*)gl->data)->send_message(GAME_STARTS);
    }
  
  // start !

  //  clear_object_list(WALL);
  

}

void
Arena::start_sequence_of_games(int number_of_games)
{
  games_remaining_in_sequence = number_of_games;
  start_game(1);
}

void
Arena::start_tournament(char** robotfilename_list, char** arenafilename_list, int robots_per_game)
{
  Robot* robotp;

  for(int i=0; robotfilename_list[i] != NULL; i++)
    {
      robotp = new Robot(robotfilename_list[i]);
      g_list_append(&all_robots_in_tournament, robotp);
    }

  for(int i=0; arenafilename_list[i] != NULL; i++)
    {
      g_list_append(&arena_filenames, g_string_new(arenafilename_list[i]));
    }

  GList* gl = g_list_first(&all_robots_in_tournament);

  for(int i=0; i<robots_per_game; i++)
    {
      g_list_append(&all_robots_in_sequence, gl->data);
      gl = g_list_next(gl);
    }
  start_sequence_of_games(1);
    
}
