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

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"

Gadget::~Gadget()
{
  if( variables != NULL )
    delete [] variables;

  if( functions != NULL )
    delete [] functions;
}


void
Gadget::init_variables( const VariableDefinition* var_def, const int last_var )
{
  variables = new Variable[last_var];

  for( int var_nr = 0; var_nr < last_var ; var_nr++)
    {
      const VariableDefinition* v = &var_def[var_nr];
      variables[var_nr] = Variable( v->name, this );
      
      variables[var_nr].set_robot_permissions( v->readable, v->writable );
      switch( v->type )
        {
        case BOOL_V:
          variables[var_nr].make_bool( v->value );
          if( v->random ) variables[var_nr].make_random( 0, 1, false );
          break;

        case INT_V:
          variables[var_nr].make_int( v->value, v->min_val, v->max_val );
          if( v->random ) variables[var_nr].make_random( v->min_val, v->max_val, false );
          break;

        case DOUBLE_V:
          variables[var_nr].make_double( v->value, v->min_val, v->max_val, v->inaccuracy );
          if( v->random ) variables[var_nr].make_random( v->min_val, v->max_val, true );
          break;
        }
    }
}

void
Gadget::init_functions( const FunctionDefinition* fcn_def, const int last_fcn )
{
  functions = new Function[last_fcn];
  
  for( int fcn_nr = 0; fcn_nr < last_fcn; fcn_nr++ )
    {
      functions[fcn_nr] = Function( fcn_def[fcn_nr].name, this );
    }

}