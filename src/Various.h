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

#ifndef __VARIOUS__
#define __VARIOUS__

#include <list>
#include <string>

struct start_tournament_info_t;

typedef void (*StartTournamentFunction)( const list<start_tournament_info_t>&,
                                         const list<start_tournament_info_t>&,
                                         const int, const int, const int, void* );

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define abs(a) ((a) > 0 ? (a) : -(a))
#define sgn(a) ((a) > 0 ? 1 : -1)

void Error(const bool fatal, const string& error_msg, const string& function_name);
void Quit(const bool success = true);

int factorial(const int n);

int binomial(const int n, const int k);

void reorder_pointer_array(void** array, int size);

void read_dirs_from_system(list<string>& robotdirs, list<string>& arenadirs);
void split_colonseparated_dirs(string& dirs, list<string>& arenadirs);

bool check_if_filename_is_robot( string& fname );
bool check_if_filename_is_arena( string& fname );
void check_for_robots_and_arenas( string& word, 
                                  list<start_tournament_info_t>& tour_list,
                                  list<string>& dir_list, 
                                  const bool check_robots );
void search_directories( string directory, list<start_tournament_info_t>& tour_list,
                         const bool check_robots );
bool parse_tournament_file( const string& fname,
                            const StartTournamentFunction function,
                            void* data, bool fatal_error_on_file_failure = true );


void create_tmp_rtb_dir();

template<class ListClass>
void clear_and_delete_pointer_list( list<ListClass*> the_list )
{
  list<ListClass*>::iterator ti;
  for( ti = the_list.begin(); ti != the_list.end(); ti++ )
    delete *ti;
  the_list.clear();
}

#endif __VARIOUS__
