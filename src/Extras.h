
#ifndef __EXTRAS__
#define __EXTRAS__

#include "Arena.h"
#include "Shape.h"

// ---------------------  Extras : ArenaObject  ---------------------

class Extras : public virtual ArenaObject
{
public:
  //Extras(const Vector2D& c, const double r); 
  //~Extras() {}
  
};

// ---------------------  Cookie : Extras  ---------------------

class Cookie : public virtual Extras, public virtual Circle
{
public:
  Cookie(const Vector2D& c, const double r, const double e); 
  ~Cookie() {}
  
  object_type get_object_type() { return COOKIE; }
  bool is_alive() { return alive; }
  void die();
  double get_energy() { return energy; }

private:
  double energy;
  bool alive;
};

// ---------------------  Mine : Extras  ---------------------

class Mine : public virtual Extras, public virtual Circle
{
public:
  Mine(const Vector2D& c, const double r, const double e); 
  ~Mine() {}

  object_type get_object_type() { return MINE; }
  bool is_alive() { return alive; }
  void die();
  double get_energy() { return energy; }

private:
  double energy;
  bool alive;
};

#endif __EXTRAS__

