#include "Arena.h"

Cookie::Cookie(const Vector2D& c, const double r, const double e) 
  : Circle(c, r), Shape(the_opts.get_l(OPTION_COOKIE_COLOUR))
{
  energy = e;
  alive = true;
}

void
Cookie::die()
{
   alive = false;
   the_gui.draw_circle(last_drawn_center,last_drawn_radius,*(the_arena.get_background_colour_p()),true);
}

Mine::Mine(const Vector2D& c, const double r, const double e)
  : Circle(c, r), Shape(the_opts.get_l(OPTION_MINE_COLOUR))
{
  energy = e;
  alive = true;
}

void
Mine::die()
{
   alive = false;
   the_gui.draw_circle(last_drawn_center,last_drawn_radius,*(the_arena.get_background_colour_p()),true);
}



