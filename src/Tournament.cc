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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "Tournament.h"
#include "Gadgets/ArenaGadget.h"


Tournament::Tournament(const int rounds_p_match,
                       const int robots_p_match,
                       const int number_o_matches,
                       const vector<Robot*>& robots,
                       const vector<ArenaGadget>& arenas) : 
  rounds_per_match(rounds_p_match), 
  robots_per_match(robots_p_match), 
  number_of_matches(number_o_matches),
  the_robots(robots),
  the_arenagadgets(arenas)
{
  create_matches();
  prepare_for_new_match();
}



// TODO: Copied from ArenaRealTime.cc:start_tournament. Needs revision.

void 
Tournament::create_matches()
{
  

  // Find robots to compete in the matches
//    int games_per_round = binomial(total_number_of_robots, robots_per_match);
//    int complete_rounds = number_of_matches / rounds_per_match;
//    int rem_games = number_of_matches % rounds_per_match;

//    robots_in_sequence = new (int*)[number_of_matches];
//    for(int i=0; i<number_of_matches; i++) robots_in_sequence[i] = new int[robots_per_game];
  
//    int current_sequence[robots_per_game];
//    int current_nr = 0;
//    //  for(int i=0; i<robots_per_game; i++) current_sequence[i] = i+1;
  
//    // set complete rounds first

//    for(int round_nr=0; round_nr < complete_rounds; round_nr++)
//      {
//        int k, i, j;

//        for(i=0; i<robots_per_game; i++) current_sequence[i] = i+1;
//        current_sequence[robots_per_game-1]--;   // To be increased first time

      
//        for(i=0; i< rounds_per_match; i++)
//          {
          
//            k = robots_per_game - 1;
//            while( current_sequence[k] == number_of_robots + 1 - robots_per_game + k )
//              k--;

//            if( k < 0 ) Error(true, "Problem generating list of participants, k < 0", 
//                              "ArenaRealTime::start_tournament");

//            current_sequence[k]++;
//            for(j=k+1; j<robots_per_game; j++) current_sequence[j] = current_sequence[j-1]+1;

//            for(j=0; j<robots_per_game; j++) 
//              robots_in_sequence[current_nr][j] = current_sequence[j];

//            current_nr++;
//          }
//        reorder_pointer_array((void**)robots_in_sequence, rounds_per_match);
//      }

//    // the remaining game will be randomly chosen

//    int robot_matches_played[number_of_robots];
//    for(int i=0; i<number_of_robots; i++) robot_matches_played[i] = 0;

//    bool robot_playing_this_match[number_of_robots];
//    int min_matches_played = 0;
//    int number_of_robots_on_min_matches = number_of_robots;
//    int nr;

//    for(int i=0; i< rem_games; i++)
//      {
//        for(int i2=0; i2<number_of_robots; i2++) robot_playing_this_match[i2] = false;

//        for(int j=0; j<robots_per_game; j++)
//          {
//            do 
//              nr = (int)floor(number_of_robots*((double)rand() / (double)RAND_MAX));
//            while( robot_playing_this_match[nr] || robot_matches_played[nr] != min_matches_played );

//            robot_playing_this_match[nr] = true;
//            robot_matches_played[nr]++;
//            number_of_robots_on_min_matches--;
//            if( number_of_robots_on_min_matches == 0 ) 
//              {
//                min_matches_played++;
//                number_of_robots_on_min_matches = number_of_robots;
//              }

//            robots_in_sequence[current_nr][j] = nr + 1;   // robot count will start from 1
//          }      
//        current_nr++;
//      }

  prepare_for_new_match();
}

void 
Tournament::prepare_for_new_match()
{
}
