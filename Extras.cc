#include "Arena.h"

Cookie::Cookie(const Vector2D& c, const double r, const double e, Arena* ap) 
  : Circle(c, r), Shape(opts.get_cookie_colour()), ArenaObject(ap)
{
  energy = e;
  alive = true;
}

void
Cookie::die()
{
   alive = false;
   the_arena->get_the_gui()->draw_circle(last_drawn_center,last_drawn_radius,*(the_arena->get_background_colour_p()),true);
}

Mine::Mine(const Vector2D& c, const double r, const double e, Arena* ap) 
  : Circle(c, r), Shape(opts.get_mine_colour()), ArenaObject(ap)
{
  energy = e;
  alive = true;
}

void
Mine::die()
{
   alive = false;
   the_arena->get_the_gui()->draw_circle(last_drawn_center,last_drawn_radius,*(the_arena->get_background_colour_p()),true);
}



