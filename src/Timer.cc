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

#include <assert.h>

#include "Timer.h"


Timer::Timer()
{
  reset();
}

void
Timer::reset()
{
  gettimeofday(&start, NULL);
  gametime = 0.0;
  gamespeed = 1.0;
}

double
Timer::update()
{
  timeval current_time;
  gettimeofday(&current_time, NULL);

  if (start.tv_usec > current_time.tv_usec)
    {
      current_time.tv_usec += 1000000;
      current_time.tv_sec--;
    }

  double elapsed;

  elapsed = ((double)( current_time.tv_usec - start.tv_usec )) / 1e6;
  elapsed += (double)( current_time.tv_sec  - start.tv_sec );


  gametime += ( elapsed - time_last_gametime_update ) * gamespeed;

  time_last_gametime_update = elapsed;

  return elapsed;
}

double
Timer::get_realtime()
{
  return update();
}

double
Timer::get_gametime()
{
  update();
  return gametime;
}

void
Timer::set_game_speed( const double speed )
{
  update();
  assert( speed >= 0.0 );
  gamespeed = speed;
}

double
Timer::realtime2gametime( const double time )
{
  double current_realtime = update();

  return gametime + ( time - current_realtime ) * gamespeed;
}

double
Timer::gametime2realtime( const double gtime )
{
  double current_realtime = update();

  return current_realtime + ( gtime - gametime ) / ( gamespeed + 1e-10 );
}

void
Timer::double2timeval( const double time, struct timeval& wait_time)
{
  double time_diff = update() - time;

  if( time_diff <= 0.0 )
    {
      wait_time.tv_sec = 0;
      wait_time.tv_usec = 0;
    }
  else
    {
      wait_time.tv_sec = int( time_diff );
      wait_time.tv_usec = int( 1e6 * ( time_diff - (double)wait_time.tv_sec ) );
    }
}
