/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <string>
#include <vector>
#include <math.h>
#include <fstream.h>
#include <strstream>

#include "Tournament.h"
#include "ArenaGadget.h"
#include "Structs.h"
#include "String.h"
#include "Various.h"
#include "EventHandler.h"
#include "EventRT.h"
#include "TournamentAgreementPackets.h"
#include "TournamentPackets.h"
#include "ServerSocket.h"
#include "Arena.h"

Tournament::Tournament() :
  robots_per_match(0),
  number_of_matches(0)
{

}

Tournament::Tournament(const tourn_info_t& new_tournament_info ) :
  my_tournament_info(new_tournament_info)
{
  //Tell to run all the robots !
  //TODO : Start
  //start();
}

Tournament::Tournament(const int robots_p_match,
                       const int number_o_matches,
                       const vector<string>& robot_filenames,
                       const vector<string>& arena_filenames) : 
  robots_per_match(robots_p_match), 
  number_of_matches(number_o_matches)
{
  //start();  //Really ?
}

Tournament::Tournament(const string& tournament_file)
{
  load_succeeded = parse_tournament_file( tournament_file );

  if( load_succeeded )
    {
      //start();  //Really ? Without any robot connected to it ?
    }

  cout<<"End of Tournament::Tournament\n";
}

void
Tournament::set_packet_factories(TournamentRobotPacketFactory* robots,
				 TournamentViewPacketFactory* views)
{
  robot_packet_factory = robots;
  view_packet_factory  = views;
}

bool 
Tournament::set_number_o_matches(int i)
{
  if( ! started )
    {
      number_of_matches = i;
      return true;
    }
  else
    {
      return false;
    }
}

bool
Tournament::set_robots_p_match(int i)
{
  if( ! started )
    {
      robots_per_match = i;
      return true;
    }
  else
    {
      return false;
    }
}


void
Tournament::start()
{

  match_nr = 0;
  //create_matches();
  //prepare_for_new_match();  
  
  //Tell the NetConnection to run the robots
  for(list<robot_info_t>::iterator ri = my_tournament_info.robots.begin(); 
      ri != my_tournament_info.robots.end(); ri ++)
    {
      int rand_id = rand()%256;
      //Use a packet to do it !!!
      ostrstream unique_name, os; 
      unique_name<<ri->filename<<"_"<<ri->id<<"/"<<rand_id<<ends; 

      //TODO : change this !!!
      os<<"RunRobot "<<ri->filename<<" "<<ri->directory<<" "<<ri->id<<" "<<rand_id<<ends;
      TournamentCommitChangePacket P( os.str() );
      ri->nc->send_data( P.make_netstring() );

      the_robots.insert(new Robot(unique_name.str(), ri->id));
    }

  //Load all arenafiles.
  for(list<arena_info_t>::iterator ai = my_tournament_info.arenas.begin();
      ai != my_tournament_info.arenas.end(); ai ++)
    {
      if(ai->selected)
	{
	  Arena * my_arena = new Arena( ai->directory+ai->filename, the_robots);
	  delete my_arena;
	  //TODO : load the arena file !
	  break;
	}
    }
  
  started = true;
}

Robot*
Tournament::connect_to_robot(NetConnection* nc, string& uniqueness_name)
{
  for( set<Robot*>::iterator ri = the_robots.begin(); ri != the_robots.end(); ri ++ )
    {
      if((*ri)->get_robot_name() == uniqueness_name && (*ri)->set_connection( nc )) {
	return *ri;
      }
    }
  return NULL;
}

void 
Tournament::prepare_for_new_match()
{
  // TODO: Startup all robots and remove all robots that failed to startup.
  //       Copy the newly started robots to the_robots.
  // NOTE: Maybe some robots are allready started : 
  //         just tell them that we are about to start a new match. 

  // TODO: Create the_arena

  match_nr++;
  the_eventhandler.insert_RT_event( new StartNewMatchEvent( 1.0, &the_matches[match_nr] ) );
}


// reorders the matches, so that all robots have almost the
//  same number of played matches after each match.
void
Tournament::reorder_matches(const int start, int end)
{
  int matchnr, i, j;
  
  int best_match;
  double best_score;
  double mean_matches;
  double score;

  int matches_for_robot[total_number_of_robots];
  for( i=0; i<total_number_of_robots; i++) matches_for_robot[i] = 0;

  for( matchnr = start; matchnr<end; matchnr++)
    {
      best_match = -1;
      best_score = 10000;
      mean_matches = robots_per_match * ( matchnr - start + 1) / total_number_of_robots;

      // For each of the remaining matches, find the match which gives the
      // smallest variation in the number of matches for each robot.

      for( i = matchnr; i<=end; i++ )
        {
          // Add a match for robots in the match i temporary
          for(j=0; j<robots_per_match; j++) 
            matches_for_robot[the_matches[i].robot_ids[j]]++;

          // Caluclate score as the sum of the distances to the mean
          score = 0.0;         
          for(j=0; j<total_number_of_robots; j++)
            score += abs( matches_for_robot[j] - mean_matches );

          for(j=0; j<robots_per_match; j++) 
            matches_for_robot[the_matches[i].robot_ids[j]]--;

          // Lowest score is stored
          if( score < best_score )
            {
              best_score = score;
              best_match = i;
            }
        }
      
      assert( best_match != -1 );

      // Add a match for robots in the match found
      for(j=0; j<robots_per_match; j++) 
        matches_for_robot[the_matches[best_match].robot_ids[j]]++;


      // Swap the matches
      if( best_match != matchnr )
        {
          Match tmp = the_matches[best_match];
          the_matches[best_match] = the_matches[matchnr];
          the_matches[matchnr] = tmp;
        }        
    }

}

void 
Tournament::create_matches()
{
  

  // Find robots to compete in the matches
  int matches_per_round = binomial(total_number_of_robots, robots_per_match);
  int complete_rounds = number_of_matches / matches_per_round;
  int rem_games = number_of_matches % matches_per_round;

  int current_sequence[robots_per_match];
  int current_match = 0;
  
  the_matches.resize( number_of_matches );
  
  for(int i=0; i < number_of_matches; i++) 
    the_matches[i].robot_ids.resize( robots_per_match );
  
  // set complete rounds first

  for(int round_nr=0; round_nr < complete_rounds; round_nr++)
    {
      int k, i, j;

      for(i=0; i<robots_per_match; i++) 
        current_sequence[i] = i+1;

      current_sequence[robots_per_match-1]--;   // To be increased first time

      
      for(i=0; i< matches_per_round; i++)
        {
          
          k = robots_per_match - 1;
          while( current_sequence[k] == total_number_of_robots + 1 - robots_per_match + k )
            k--;

          if( k < 0 ) Error(true, "Problem generating list of participants, k < 0", 
                            "ArenaRealTime::start_tournament");

          current_sequence[k]++;
          for(j=k+1; j<robots_per_match; j++) current_sequence[j] = current_sequence[j-1]+1;

          for(j=0; j<robots_per_match; j++)
            the_matches[current_match].robot_ids[j] = current_sequence[j];           

          current_match++;
        }

      reorder_matches(current_match - matches_per_round, current_match - 1);
    }

  // the remaining games will be randomly chosen

  int first_of_remaining_games = current_match;

  int robot_matches_played[total_number_of_robots];
  for(int i=0; i<total_number_of_robots; i++) robot_matches_played[i] = 0;

  bool robot_playing_this_match[total_number_of_robots];
  int min_matches_played = 0;
  int number_of_robots_on_min_matches = total_number_of_robots;
  int nr;

  for(int i=0; i< rem_games; i++)
    {
      for(int i2=0; i2<total_number_of_robots; i2++) robot_playing_this_match[i2] = false;

      for(int j=0; j<robots_per_match; j++)
        {
          do 
            nr = (int)floor(total_number_of_robots*((double)rand() / (double)RAND_MAX));
          while( robot_playing_this_match[nr] || robot_matches_played[nr] != min_matches_played );

          robot_playing_this_match[nr] = true;
          robot_matches_played[nr]++;
          number_of_robots_on_min_matches--;
          if( number_of_robots_on_min_matches == 0 ) 
            {
              min_matches_played++;
              number_of_robots_on_min_matches = total_number_of_robots;
            }

            the_matches[current_match].robot_ids[j] = nr + 1;
        }      
      current_match++;
    }

  reorder_matches(first_of_remaining_games, current_match - 1);

  prepare_for_new_match();
}


bool
Tournament::parse_tournament_file( const string& filename )
{
  list<string> robotdirs;
  list<string> arenadirs;

  //  read_dirs_from_system(robotdirs, arenadirs);

  bool fatal_error_on_file_failure = false;

  ifstream file(filename.c_str());

  if( !file )
    {
      cout<<"No such file\n";
      if( fatal_error_on_file_failure )
        Error( true, "Can't open specified tournament file.",
               "parse_tournament_file" );
      else
        return false;
      
    }

  int games_p_s = 1;
  int robots_p_s = 2;
  int n_o_sequences = 1;
  int looking_for = 0; // 0 = keyword, 1 = robot, 2 = arena

  list<start_tournament_info_t> robot_list;
  list<start_tournament_info_t> arena_list;

  for(;;)
    {
      char buffer[200];
      file >> buffer;
      string word(buffer);

      if( word == "" )
        {
          int robots_counted = robot_list.size();
          int arenas_counted = arena_list.size();
          
          if (games_p_s == -1)
            games_p_s = arenas_counted;

          if (robots_p_s == -1)
            robots_p_s = robots_counted;

          if (n_o_sequences == -1)
            n_o_sequences=binomial(robots_counted, games_p_s);

          robots_p_s = min(robots_counted,robots_p_s);
          
          if(robots_p_s < 2)
            {
              if( fatal_error_on_file_failure )
                Error(true, "Can't start tournament with only " +
                      int2string(robots_p_s) + " robots per sequence", 
                      "parse_tournament_file");
              else
                return false;
            }

          if(games_p_s < 1)
            {
              if(fatal_error_on_file_failure)
                Error(true, "Must have at least one game per sequence. " 
                      "Current value is: " + int2string(games_p_s),
                      "parse_tournament_file");
              else
                return false;
            }

          if(n_o_sequences < 1)
            {
              if(fatal_error_on_file_failure)
                Error(true, "Must have at least one sequence. Current value is: " + 
                      int2string(n_o_sequences), "parse_tournament_file");
              else
                return false;
            }

          // Startup the tournament

          //          (*function)( robot_list, arena_list, robots_p_s, 
          //                       games_p_s, n_o_sequences, data );

          return true;
        }


      if( (equal_strings_nocase(word,"games/sequence:")) || 
          (equal_strings_nocase(word,"g/s:") ) )
        {
          looking_for = 0;
          file >> buffer;
          if( buffer[0] == '*' )
            games_p_s = -1;
          else
            games_p_s = string2int( buffer );
        }
      else if( (equal_strings_nocase(word,"robots/sequence:")) || 
               (equal_strings_nocase(word,"r/s:")) )
        {
          looking_for = 0;
          file >> buffer;
          if( buffer[0] == '*' )
            robots_p_s = -1;
          else
            robots_p_s = string2int( buffer );
        }
      else if( (equal_strings_nocase(word,"sequences:")) || 
               (equal_strings_nocase(word,"seq:")) )
        {
          looking_for = 0;
          file >> buffer;
          if( buffer[0] == '*' )
            n_o_sequences = -1;
          else
            n_o_sequences = string2int( buffer );
        }
      else if( (equal_strings_nocase(word,"robots:")) ||
               (equal_strings_nocase(word,"r:")) )
        looking_for = 1;
      else if( (equal_strings_nocase(word,"arenas:")) ||
               (equal_strings_nocase(word,"a:")) )
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
              check_for_robots_and_arenas( word, robot_list, robotdirs, true);
              break;
            case 2:
              check_for_robots_and_arenas( word, arena_list, arenadirs, false);
              break;
            }
        }
    }
  return false;
}
