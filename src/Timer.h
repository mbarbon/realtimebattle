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

#ifndef __TIMER__
#define __TIMER__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef TIME_WITH_SYS_TIME 
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

class Timer
{
public:
  
  Timer();
  ~Timer() {}

  double get_realtime();   //Return the elapsed time [in sec] since the begining/last_reset
  double get_gametime();

  double realtime2gametime(const double time);
  double gametime2realtime(const double gtime);

  void double2timeval( const double time, struct timeval& wait_time);

  void reset();   //Reset the timer (how to be useless :) )
  
  void set_game_speed( const double speed );
  double get_game_speed() const { return gamespeed; }

  unsigned int get_random_seed() { return (unsigned int)start.tv_usec; }

private:
  
  double update();

  struct timeval start;

  double gametime;
  double gamespeed;
  
  double current_time;
};

#endif __TIMER__
