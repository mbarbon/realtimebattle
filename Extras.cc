#include "Arena.h"

Cookie::Cookie(const Vector2D& c, const double r, const double e)
{
  my_shape = Circle(c, r);
  energy = e;
}

int
Cookie::touch_action(class Robot& robot)
{
  robot.change_energy(energy);
  return( 1 );
}

