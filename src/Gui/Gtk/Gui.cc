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
#include <stdlib.h>

#include <typeinfo>

#include "Gui.h"
#include "GuiInterface.h"
#include "IntlDefs.h"
#include "ArenaController.h"
#include "Wall.h"
#include "ArenaWindow.h"
#include "MessageWindow.h"
#include "ScoreWindow.h"
#include "ControlWindow.h"
#include "StatisticsWindow.h"
#include "StartTournamentWindow.h"
#include "Dialog.h"
#include "DrawingObjects.h"
#include "List.h"
#include "Options.h"
#include "String.h"
#include "GuiVarious.h"

class Gui* gui_p;

//Move this into gui sometime soon.
class ControlWindow* controlwindow_p;

const String
GIName()
{
  static const String name(_("Gtk+ based GUI"));
  return name;
}

const String
GIUsageMessage()
{
  static const String usage(_("No options"));
  return usage;
}

bool
GIInit( int argc, char** argv )
{
#ifdef HAVE_LOCALE_H
  gtk_set_locale();
#endif

  gtk_init (&argc, &argv);

  gui_p = new Gui();
  return (gui_p != NULL);
}
//TODO: How do we get the_opts?
int
GIMain( GuiInterface* _gi_p )
{
  int returncode = the_gui.main_loop( _gi_p );
  //  _gi_p->quit(); //TODO: How do we tell that RTB should Quit?
  return returncode;
}

Gui::Gui()
{
  arenawindow_p = NULL;
  messagewindow_p = NULL;
  scorewindow_p = NULL;
  statisticswindow_p = NULL;
  starttournamentwindow_p = NULL;
}

int
Gui::main_loop( GuiInterface* _gi_p )
{
  set_colours();
  controlwindow_p = 
    new ControlWindow( -1, -1,
                       the_opts.get_l( OPTION_CONTROL_WINDOW_POS_X ),
                       the_opts.get_l( OPTION_CONTROL_WINDOW_POS_Y ) );

  gint timeout_tag;      
  double interval = 1000.0*the_opts.get_d( OPTION_UPDATE_INTERVAL ) - 10.0; 
  timeout_tag = gtk_timeout_add( (unsigned int) interval,
                                 GtkFunction(Gui::update_function), (gpointer) NULL );

  gtk_main();

  return EXIT_SUCCESS;
}

int
Gui::timeout_function()
{
  //  update_lists();
  return true;
}

void
Gui::update_lists()
{
  List<Shape>* object_lists;
  object_lists = the_arena.get_object_lists();

  ListIterator<Shape> li;

  for( int obj_type=ROBOT; obj_type < LAST_OBJECT_TYPE; obj_type++ )
    {
      list<DrawingShape*>::iterator draw_li = drawing_objects_lists[obj_type].begin();
      for( object_lists[obj_type].first(li); li.ok(); li++ )
        {
          if( draw_li != drawing_objects_lists[obj_type].end() )
            {
              if( (*draw_li)->get_id() != li()->get_id() )
                //TODO: remove the object from arena.
                {
                  delete (*draw_li);
                  drawing_objects_lists[obj_type].erase(draw_li);
                }
              draw_li++;
            }
          else
            {
              switch( obj_type )
                {
                case ROBOT:
                  {
                    DrawingShape* p = (DrawingShape*) new DrawingCircle( li() );
                    drawing_objects_lists[obj_type].push_back( p );
                  }
                  break;
                case SHOT:
                  {
                    DrawingShape* p = (DrawingShape*) new DrawingCircle( li() );
                    drawing_objects_lists[obj_type].push_back( p );
                  }
                  break;
                case WALL:
                  if( typeid( *li() ) == typeid( WallLine ) )
                    {
                      DrawingShape* p = (DrawingShape*) new DrawingLine( li() );
                      drawing_objects_lists[obj_type].push_back( p );
                    }
                  else if( typeid( *li() ) == typeid( WallCircle ) )
                    {
                      DrawingShape* p = (DrawingShape*) new DrawingCircle( li() );
                      drawing_objects_lists[obj_type].push_back( p );
                    }
                  else if( typeid( *li() ) == typeid( WallInnerCircle ) )
                    {
                      DrawingShape* p = (DrawingShape*) new DrawingInnerCircle( li() );
                      drawing_objects_lists[obj_type].push_back( p );
                    }
                  break;
                case COOKIE:
                  {
                    DrawingShape* p = (DrawingShape*) new DrawingCircle( li() );
                    drawing_objects_lists[obj_type].push_back( p );
                  }
                  break;
                case MINE:
                  {
                    DrawingShape* p = (DrawingShape*) new DrawingCircle( li() );
                    drawing_objects_lists[obj_type].push_back( p );
                  }
                  break;
                }
            }
        }
    }
}

void
Gui::set_colours()
{  
  bg_rgb_colour = the_opts.get_l( OPTION_BACKGROUND_COLOUR );
  fg_rgb_colour = the_opts.get_l( OPTION_FOREGROUND_COLOUR );
  rtb_message_rgb_colour = the_opts.get_l( OPTION_RTB_MESSAGE_COLOUR );

  bg_gdk_colour = make_gdk_colour( bg_rgb_colour );
  fg_gdk_colour = make_gdk_colour( fg_rgb_colour );
  rtb_message_gdk_colour = make_gdk_colour( rtb_message_rgb_colour );
}

void
Gui::open_arenawindow()
{
  if( NULL == arenawindow_p )
    arenawindow_p = 
      new ArenaWindow( the_opts.get_l( OPTION_ARENA_WINDOW_SIZE_X ),
                       the_opts.get_l( OPTION_ARENA_WINDOW_SIZE_Y ),
                       the_opts.get_l( OPTION_ARENA_WINDOW_POS_X  ),
                       the_opts.get_l( OPTION_ARENA_WINDOW_POS_Y  ) );
}

void
Gui::close_arenawindow()
{
  if( NULL != arenawindow_p )
    {
      delete arenawindow_p;
      arenawindow_p = NULL;
    }
}

void
Gui::open_messagewindow()
{
  if( NULL == messagewindow_p )
    messagewindow_p = 
      new MessageWindow( the_opts.get_l( OPTION_MESSAGE_WINDOW_SIZE_X ),
                         the_opts.get_l( OPTION_MESSAGE_WINDOW_SIZE_Y ),
                         the_opts.get_l( OPTION_MESSAGE_WINDOW_POS_X ),
                         the_opts.get_l( OPTION_MESSAGE_WINDOW_POS_Y ) );
}

void
Gui::close_messagewindow()
{
  if( NULL != messagewindow_p )
    {
      delete messagewindow_p;
      messagewindow_p = NULL;
    }
}

void
Gui::open_scorewindow()
{
  if( NULL == scorewindow_p )
    scorewindow_p = 
      new ScoreWindow( the_opts.get_l( OPTION_SCORE_WINDOW_SIZE_X ),
                       the_opts.get_l( OPTION_SCORE_WINDOW_SIZE_Y ),
                       the_opts.get_l( OPTION_SCORE_WINDOW_POS_X ),
                       the_opts.get_l( OPTION_SCORE_WINDOW_POS_Y ) );
}

void
Gui::close_scorewindow()
{
  if( NULL != scorewindow_p )
    {
      delete scorewindow_p;
      scorewindow_p = NULL;
    }
}

void
Gui::open_statisticswindow()
{
  if( the_arena_controller.is_started() )
    if( NULL == statisticswindow_p && the_arena.get_state() != NOT_STARTED )
      statisticswindow_p = 
        new StatisticsWindow( the_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_X ),
                              the_opts.get_l( OPTION_STATISTICS_WINDOW_SIZE_Y ),
                              -1, -1 );
}

void
Gui::close_statisticswindow()
{
  if( NULL != statisticswindow_p )
    {
      delete statisticswindow_p;
      statisticswindow_p = NULL;
    }
}

void
Gui::open_starttournamentwindow()
{
  bool open = false;
  if( NULL == starttournamentwindow_p )
    {
      if( the_arena_controller.is_started() )
        if( the_arena.get_state() != NOT_STARTED &&
            the_arena.get_state() != FINISHED )
          {
            String info_text = _("This action will kill the current tournament.\n"
                                 "Do you want to do that?");
            list<String> string_list;
            string_list.push_back( String( _("Yes") ) );
            string_list.push_back( String( _("No")  ) );
            Dialog( info_text, string_list,
                    (DialogFunction) Gui::kill_and_start_new_tournament );
          }
        else
          open = true;
      else
        open = true;
    }

  if( open )
    starttournamentwindow_p = 
      new StartTournamentWindow( -1, -1, -1, -1 );
}

void
Gui::kill_and_start_new_tournament( int result )
{
  if( the_arena_controller.is_started() && result == 1 )
    {
      the_arena.interrupt_tournament();
      the_gui.open_starttournamentwindow();
    }
}

void
Gui::close_starttournamentwindow()
{
  if( NULL != starttournamentwindow_p )
    {
      delete starttournamentwindow_p;
      starttournamentwindow_p = NULL;
    }
}

gint
Gui::update_function(gpointer data)
{  
  gint res = 1;

  res = (gint)the_gui.timeout_function();

  return res;
}
