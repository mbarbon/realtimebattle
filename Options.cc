#include <math.h>
#include "Options.h"

// void
// Options::set_options()
// {
// }

Options::Options()
{
  grav_const = 9.82;
  air_resistance = 0.005;
  roll_friction = 0.002;
  slide_friction = 0.1;
  max_acceleration = 2.0;
  min_acceleration = -0.5;
    
  robot_radius = 0.5;
  robot_mass = 1.0;
  robot_bounce_coeff = 0.7;
  robot_hardness = 0.5;
  robot_protection = 0.5;

  robot_start_energy = 100.0;
  robot_max_energy = 120.0;

  robot_front_protection = 0.9;
  robot_front_hardness = 0.9;
  robot_front_bounce_coeff = 0.7;
  robot_cos_frontsize = cos(M_PI/6.0);   // 2*30 degrees

  robot_max_rotate = 2.0*M_PI / 8.0;   // [rad / s]
  robot_cannon_max_rotate = 2.0*M_PI / 4.0;   // [rad / s]
  robot_radar_max_rotate = 2.0*M_PI / 3.0;   // [rad / s]
    
  shot_radius = 0.1;
  shot_speed = 10.0;
  shooting_penalty = 0.075;
  shot_min_energy = 0.5;
  shot_max_energy = 30.0;
  shot_energy_increase_speed = 10.0;   // [energypoints / s]

  background_colour = 0xfaf0e6;  // linen
  foreground_colour = 0x000000;   // black

  cookie_max_energy = 15.0;
  cookie_min_energy = 10.0;
  cookie_frequency = 1.0/30.0;
  cookie_radius = 0.3;
  cookie_colour = 0x35d715;

  mine_max_energy = 25.0;
  mine_min_energy = 15.0;
  mine_frequency = 1.0/15.0;
  mine_radius = 0.3;
  mine_colour = 0xff0000;

  max_timestep = 0.5;
  timeout = 120.0;        // i.e. 2 minutes
}



