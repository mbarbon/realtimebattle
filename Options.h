

#ifndef __OPTIONS__
#define __OPTIONS__

class Options
{
public:
  Options();
  ~Options() {}
  //static void set_options();

private:
   double grav_const; // = 9.82;
   double air_resistance; // = 0.005;
   double roll_friction; // = 0.002;
   double slide_friction; // = 0.1;
   double max_acceleration; // = 2.0;
   double min_acceleration; // = -0.5;

   double robot_radius; // = 0.5;
   double robot_mass; // = 1.0;
   double robot_bounce_coeff; // = 0.7;
   double robot_hardness; // = 0.5;
   double robot_protection; // = 0.5;

   double shot_radius; // = 0.1;
   double shot_speed; // = 10.0;
   double start_energy; // = 100.0;
   double shooting_penalty; // = 0.075;
  
   long background_colour; // = 0xfaf0e6;  // linen
   long foreground_colour; // = 0x000000;   // black

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

    double get_shot_radius() { return shot_radius; }
    double get_shot_speed() { return shot_speed; }
    double get_start_energy() { return start_energy; }
    double get_shooting_penalty() { return shooting_penalty; }

   long get_background_colour() { return Options::background_colour; }
   long get_foreground_colour() { return foreground_colour; }
};
#endif __OPTIONS__
