/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <string>
#include "String.h"

#include "Script.h"
#include "Value.h"
#include "EventHandler.h"
#include "EventGT.h"
#include "Variable.h"

void Error(const bool fatal, const string& function_name, const string& error_msg);

Value&
Script::ScriptLine::get_value()
{
  if( is_variable )
    {
      //      val = 0.0; // Not !
    }

  return val;
}

const Script& 
Script::operator= (const Script& v) {
  commands = v.commands;
}

/*const ShotGadget& 
ShotGadget::operator= (const ShotGadget& v) {
  for(int i = 0; i < LAST_SHOTVAR; i++)
    variables[i] = (Value&) v.variables[i];
  return (*this);
  }*/

void
Script::run()
{
  while ( !val_stack.empty() ) val_stack.pop();  // clear stack


  current_line = commands.begin();
  list<ScriptLine>::iterator next_line = current_line;

  for(; next_line != commands.end(); )
    {
      current_line = next_line;
      next_line++;

      switch( (*current_line).function )
        {
        case PUSH:
          val_stack.push( (*current_line).get_value() );
          break;

        case POP:
          val_stack.pop();
          break;

        case COPY:
          val_stack.push( val_stack.top() );
          break;

        case STORE:
          assert( (*current_line).is_variable );
          (*current_line).var_p->assign((*current_line).get_value());
          break;

        case ACTION:
          // eval the string *str_p
          break;

        case ADD:
          val_stack.top() += (*current_line).get_value();
          break;

        case SUB:
          val_stack.top() -= (*current_line).get_value();
          break;

        case MULT:
          val_stack.top() *= (*current_line).get_value();
          break;

        case DIV:
          val_stack.top() /= (*current_line).get_value();
          break;

        case INV:
          val_stack.top().invert();
          break;

        case NEG:
          val_stack.top().neg();
          break;

        case SIN:
          val_stack.top().sin();
          break;

        case COS:
          val_stack.top().cos();
          break;

        case TAN:
          val_stack.top().tan();
          break;

        case ASIN:
          val_stack.top().asin();
          break;

        case ACOS:
          val_stack.top().acos();
          break;

        case ATAN:
          val_stack.top().atan();
          break;

        case LT:
          val_stack.top() < (*current_line).get_value();
          break;

        case LE:
          val_stack.top() <= (*current_line).get_value();
          break;

        case GT:
          val_stack.top() > (*current_line).get_value();
          break;

        case GE:
          val_stack.top() >= (*current_line).get_value();
          break;

        case EQ:
          val_stack.top() == (*current_line).get_value();
          break;

        case NE:
          val_stack.top() != (*current_line).get_value();
          break;

        case THEN:
          if( ! int(val_stack.top()) ) next_line++;
          break;

        case ELSE:
          if( int(val_stack.top()) ) next_line++;
          break;

        case GOTO:
          {
            int l = int((*current_line).get_value());
            list<ScriptLine>::iterator li = commands.begin();
              for( ; li != commands.end(); li ++ )
                {
                  if( (*li).line_nr == l )
                    {
                      next_line = li;
                      break;
                    }
                }
              Error(true, "Couldn't goto line " + int2string(l), "Script::run");
          }
          break;
          
        case WAIT:
          {
            current_line = next_line;
            next_line = commands.end(); // to finish loop
            
            the_eventhandler.insert_GT_event
              ( new ContinueScriptEvent( double((*current_line).get_value()), this ) );

            pausing = true;
          }
          break;

//          default:
//            Error(true, "Sorry, that function (number " + int2string((*current_line).function) +
//                  " is not implemented", "Script::run");
        }


    } 

  
}
