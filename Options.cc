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
    
  shot_radius = 0.1;
  shot_speed = 10.0;
  start_energy = 100.0;
  shooting_penalty = 0.075;
  
  background_colour = 0xfaf0e6;  // linen
  foreground_colour = 0x000000;   // black
}


//  double Options::get_grav_const() { return grav_const; }
//  double Options::get_air_resistance() { return air_resistance; }
//  double Options::get_roll_friction() { return roll_friction; }
//  double Options::get_slide_friction() { return slide_friction; }
//  double Options::get_max_acceleration() { return max_acceleration; }
//  double Options::get_min_acceleration() { return min_acceleration; }

//  double Options::get_robot_radius() { return robot_radius; }
//  double Options::get_robot_mass() { return robot_mass; }
//  double Options::get_robot_bounce_coeff() { return robot_bounce_coeff; } 
//  double Options::get_robot_hardness() { return robot_hardness; }
//  double Options::get_robot_protection() { return robot_protection; }

//  double Options::get_shot_radius() { return shot_radius; }
//  double Options::get_shot_speed() { return shot_speed; }
//  double Options::get_start_energy() { return start_energy; }
//  double Options::get_shooting_penalty() { return shooting_penalty; }

//  long Options::get_background_colour() { return background_colour; }
//  long Options::get_foreground_colour() { return foreground_colour; }




