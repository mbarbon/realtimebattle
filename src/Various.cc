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
#include <config.h>
#endif

#include <math.h>
#include <limits.h>

#include <sys/stat.h>
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
#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>

#include "Various.h"
#include "IntlDefs.h"
#include "OptionHandler.h"
#include "Structs.h"
#include "String.h"
#include "ArenaController.h"

//extern bool no_graphics;

#include "EventHandler.h"

void
Error(const bool fatal, const string& error_msg, const string& function_name)
{
  cerr << "RealTimeBattle: " << _("Error in") << " "
       << function_name << ": " << error_msg << endl;
  //  perror("RealTimeBattle: errno message");

  if( fatal == true )
    {
      the_eventhandler.quit();
    }
}

void
Quit(const bool success)
{
//  #ifndef NO_GRAPHICS
//    if( gtk_main_level() != 0 )
//    {
//      gtk_main_quit();

//      if( !no_graphics )
//        delete controlwindow_p;
//    }
//  #endif NO_GRAPHICS

  the_arena_controller.exit_all_guis(); // Necessary to join all threads!

  if( !success )
    exit(EXIT_FAILURE);  
      
  exit(EXIT_SUCCESS);
}


int
factorial(const int n)
{
  double fact = 1.0;
  for(int i=1; i<=n; i++)  
    fact *= i;

  if( fact < (double)INT_MAX )
    return (int)fact;
  else
    return INT_MAX;
}

int
binomial(const int n, const int k)
{
  int k2 = min(k, n-k);
  
  double bin = 1;
  for(int i=n; i>n-k2; i--)  
    bin *= i;

  for(int i=1; i<=k2; i++)  
    bin /= i;

  if( bin < (double)INT_MAX )
    return (int)bin;
  else
    return INT_MAX;
}

//  void 
//  reorder_pointer_array(void** array, int size)
//  {
//    int n1, n2;
//    void* temp_p;
//    for(int i=0; i<size*5; i++)
//      {
//        n1 = (int)floor(size*((double)rand() / (double)RAND_MAX)) ;
//        n2 = (int)floor(size*((double)rand() / (double)RAND_MAX)) ;
//        if( n1 != n2 )
//          {
//            temp_p = array[n1];
//            array[n1] = array[n2];
//            array[n2] = temp_p;
//          }
//      }
//  }

void
read_robotdirs_from_system(list<string>& robotdirs)
{
  string dirs;

  robotdirs.clear();

  dirs = the_opts.get_s(OPTION_ROBOT_SEARCH_PATH);
  split_colonseparated_dirs(dirs, robotdirs);

#ifdef ROBOTDIR
  robotdirs.push_back( string(ROBOTDIR "/") );
#endif
}

void
read_arenadirs_from_system(list<string>& arenadirs)
{
  string dirs;

  arenadirs.clear();

  dirs = the_opts.get_s(OPTION_ARENA_SEARCH_PATH);
  split_colonseparated_dirs(dirs, arenadirs);

#ifdef ARENADIR
  arenadirs.push_back( string(ARENADIR "/") );
#endif
}

void
read_dirs_from_system(list<string>& robotdirs, list<string>& arenadirs)
{
  read_robotdirs_from_system( robotdirs );
  read_arenadirs_from_system( arenadirs );
}

// This function splits a string of colonseparated directories into a glist
void
split_colonseparated_dirs(string& dirs, list<string>& str_list)
{
  string current_dir = dirs;
  string::size_type pos, lastpos = 0;
  while( (pos = dirs.find(':', lastpos)) != string::npos )
    {
      current_dir = dirs.substr(lastpos, pos-1);
      if(current_dir[current_dir.length() - 1] != '/')
        current_dir += '/';

      str_list.push_back( current_dir );

      lastpos = pos+1;
    }

  if(current_dir != "")
    {
      current_dir = dirs.substr(lastpos, string::npos); 
      if(current_dir[current_dir.length() - 1] != '/')
        current_dir += '/';

      str_list.push_back( current_dir );
    }
}

bool
check_if_filename_is_robot( const string& fname )
{
  struct stat filestat;
  if( 0 == stat( fname.c_str(), &filestat ) )
    if( S_ISREG( filestat.st_mode) && (filestat.st_mode & S_IXOTH) ) // Check if file is a regular file that can be executed
      return true;

  return false;
}

bool
check_if_filename_is_arena( const string& fname )
{
  struct stat filestat;
  if( 0 == stat( fname.c_str(), &filestat ) && fname.length() > 6 )
    // Check if file is a regular file that is readable and ends with .arena
    if( S_ISREG( filestat.st_mode) &&
        (filestat.st_mode & S_IROTH)  &&
        string(".arena") == fname.substr( fname.length() - 6, string::npos ) )
      return true;

  return false;
}

bool
check_if_filename_is_regular_file( const string& fname )
{
  struct stat filestat;
  if( 0 == stat( fname.c_str(), &filestat ) && fname.length() > 6 )
    // Check if file is a regular file that is readable and ends with .arena
    if( S_ISREG( filestat.st_mode) &&
        (filestat.st_mode & S_IROTH) )
      return true;

  return false;
}

void
check_for_robots_and_arenas( string& word, 
                             list<start_tournament_info_t>& tour_list,
                             list<string>& dir_list, 
                             const bool check_robots )
{
  bool found = false;
  string full_file_name = "";

  if( word.length() > 1 )
    if( word.substr( word.length() - 2, string::npos ) == "/*" )
      {
        search_directories( word.substr( 0, word.length() - 1 ), tour_list, check_robots );
        return;
      }
  if( word.length() == 1 && word[0] == '*' )
    {
      list<string>::iterator li;
      for( li = dir_list.begin(); li != dir_list.end(); li++ )
        search_directories( (*li), tour_list, check_robots );
      return;
    }
  if( word.find('/') != string::npos )
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
      list<string>::iterator li;
      for( li = dir_list.begin(); li != dir_list.end(); li++ )
        {
          string temp_name = (*li) + word;

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
      tour_list.push_back( start_tournament_info_t(0, false, full_file_name, "") );
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
search_directories( string directory, 
                    list<start_tournament_info_t>& tour_list,
                    const bool check_robots )
{
  DIR* dir;
  if( NULL != ( dir = opendir(directory.c_str()) ) )
    {
      struct dirent* entry;
      while( NULL != ( entry = readdir( dir ) ) )
        {
          string full_file_name = directory + entry->d_name;
          bool res = false;
          if(check_robots)
            res = check_if_filename_is_robot(full_file_name);
          else
            res = check_if_filename_is_arena(full_file_name);
          if(res)
            tour_list.push_back( start_tournament_info_t(0, false,
                                                         full_file_name, "") );
        }
      closedir(dir);
    }
}

void
create_tmp_rtb_dir()
{
  string dirname = the_opts.get_s( OPTION_TMP_RTB_DIR );
  struct stat filestat;
  if( 0 != stat( dirname.c_str(), &filestat ) ) 
    mkdir( dirname.c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
}
