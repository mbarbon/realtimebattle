

#ifndef __OPTIONS__
#define __OPTIONS__

class Options
{
public:
  Options();
  ~Options() {}
  //static void set_options();

private:
  double grav_const;
  double air_resistance;
  double roll_friction;
  double slide_friction;
  double max_acceleration;
  double min_acceleration;

  double robot_radius;
  double robot_mass;
  double robot_bounce_coeff;
  double robot_hardness;
  double robot_protection;

  double robot_front_protection;
  double robot_front_hardness;
  double robot_front_bounce_coeff;
  double robot_cos_frontsize;

  double shot_radius;
  double shot_speed;
  double start_energy;
  double shooting_penalty;
  
  long background_colour;
  long foreground_colour;

  double cookie_max_energy;
  double cookie_min_energy;
  double cookie_frequency;
  double cookie_radius;
  long cookie_colour;

  double mine_max_energy;
  double mine_min_energy;
  double mine_frequency;
  double mine_radius;
  long mine_colour;

public:
  double get_grav_const() { return grav_const; }
  double get_air_resistance() { return air_resistance; }
  double get_roll_friction() { return roll_friction; }
  double get_slide_friction() { return slide_friction; }
  double get_max_acceleration() { return max_acceleration; }
  double get_min_acceleration() { return min_acceleration; }

  double get_robot_radius() { return robot_radius; }
  double get_robot_mass() { return robot_mass; }
  double get_robot_bounce_coeff() { return robot_bounce_coeff; } 
  double get_robot_hardness() { return robot_hardness; }
  double get_robot_protection() { return robot_protection; }

  double get_robot_front_protection() { return robot_front_protection; }
  double get_robot_front_hardness() { return robot_front_hardness; }
  double get_robot_front_bounce_coeff() { return robot_front_bounce_coeff; }
  double get_robot_cos_frontsize() { return robot_cos_frontsize; }

  double get_shot_radius() { return shot_radius; }
  double get_shot_speed() { return shot_speed; }
  double get_start_energy() { return start_energy; }
  double get_shooting_penalty() { return shooting_penalty; }

  long get_background_colour() { return background_colour; }
  long get_foreground_colour() { return foreground_colour; }

  double get_cookie_max_energy() { return cookie_max_energy; }
  double get_cookie_min_energy() { return cookie_min_energy; }
  double get_cookie_frequency() { return cookie_frequency; }
  double get_cookie_radius() { return cookie_radius; }
  long get_cookie_colour() { return cookie_colour; }
  
  double get_mine_max_energy() { return mine_max_energy; }
  double get_mine_min_energy() { return mine_min_energy; }
  double get_mine_frequency() { return mine_frequency; }
  double get_mine_radius() { return mine_radius; }
  long get_mine_colour() { return mine_colour; }
};
#endif __OPTIONS__


