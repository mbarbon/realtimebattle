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

#include <gtk/gtk.h>

#include <list>
#include <string>
#include <algorithm>

#include "ControlWindow.h"
#include "IntlDefs.h"
#include "ArenaWindow.h"
#include "MessageWindow.h"
#include "ScoreWindow.h" 
#include "StatisticsWindow.h" 
#include "Gui.h"
#include "Dialog.h"
#include "OptionHandler.h"
#include "Structs.h"
#include "FileSelector.h"
#include "String.h"
#include "GuiVarious.h"

ControlWindow::ControlWindow( const int default_width,
                              const int default_height,
                              const int default_x_pos,
                              const int default_y_pos )
{
  // The window widget

  window_p = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_widget_set_name( window_p, "RTB Control" );
  gtk_window_set_policy( GTK_WINDOW( window_p ), FALSE, FALSE, FALSE );
  gtk_window_set_title( GTK_WINDOW( window_p ), "RealTimeBattle" );

  gtk_container_border_width( GTK_CONTAINER( window_p ), 0 );

  if( default_width != -1 && default_height != -1 )
    gtk_widget_set_usize( window_p, default_width, default_height );
  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );

  gtk_signal_connect( GTK_OBJECT( window_p ), "delete_event",
                      (GtkSignalFunc) ControlWindow::delete_event_occured,
                      (gpointer) this );

  // Main boxes

  GtkWidget* main_vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), main_vbox );
  gtk_widget_show( main_vbox );

  // The menu

  //  This is the GtkItemFactoryEntry structure used to generate new menus.
  //            Item 1: The menu path. The letter after the underscore indicates an
  //                    accelerator key once the menu is open.
  //            Item 2: The accelerator key for the entry
  //            Item 3: The callback function.
  //            Item 4: The callback action.  This changes the parameters with
  //                    which the function is called.  The default is 0.
  //            Item 5: The item type, used to define what kind of an item it is.
  //                    Here are the possible values:
  //                    NULL               -> "<Item>"
  //                    ""                 -> "<Item>"
  //                    "<Title>"          -> create a title item
  //                    "<Item>"           -> create a simple item
  //                    "<CheckItem>"      -> create a check item
  //                    "<ToggleItem>"     -> create a toggle item
  //                    "<RadioItem>"      -> create a radio item
  //                    <path>             -> path of a radio item to link against
  //                    "<Separator>"      -> create a separator
  //                    "<Branch>"         -> create an item to hold sub items
  //                    "<LastBranch>"     -> create a right justified branch

  GtkItemFactoryEntry menu_items[] =
  {
    { "/_"N_("File"), NULL, NULL, 0, "<Branch>" },
    { "/" N_("File") "/tearoff", NULL, NULL, 0, "<Tearoff>" },
    { "/" N_("File") "/" N_("New tournament"), "<control>n",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_NEW_TOURNAMENT, "" },
    { "/" N_("File") "/" N_("Replay tournament"), "<control>r",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_REPLAY_TOURNAMENT, "" },
    { "/" N_("File") "/sep1", NULL, NULL, 0, "<Separator>" },
    { "/" N_("File") "/" N_("Specify logfile"), NULL,
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_LOGFILE, "" },
    { "/" N_("File") "/" N_("Specify messagefile"), NULL,
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_MESSAGEFILE, "" },
    { "/" N_("File") "/sep2", NULL, NULL, 0, "<Separator>" },
    { "/" N_("File") "/" N_("Quit"), "<control>q",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_QUIT, "" },
    { "/_" N_("Windows"), NULL, NULL, 0, "<Branch>" },
    { "/" N_("Windows") "/tearoff", NULL, NULL, 0, "<Tearoff>" },
    { "/" N_("Windows") "/" N_("Options"), "<control>o",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_OPTIONS, "" },
    { "/" N_("Windows") "/" N_("Statistics"), "<control>t",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_STATISTICS, "" },
    { "/" N_("Windows") "/sep", NULL, NULL, 0, "<Separator>" },
    { "/" N_("Windows") "/" N_("Show arena window"), "<control>a",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_SHOW_ARENA, "<CheckItem>" },
    { "/" N_("Windows") "/" N_("Show message window"), "<control>m",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_SHOW_MESSAGES, "<CheckItem>" },
    { "/" N_("Windows") "/" N_("Show score window"), "<control>s",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_SHOW_SCORE, "<CheckItem>" },
    { "/_" N_("Match control"), NULL, NULL, 0, "<Branch>" },
    { "/" N_("Match control") "/tearoff", NULL, NULL, 0, "<Tearoff>" },
    { "/" N_("Match control") "/" N_("Pause"), "<shift>p",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_PAUSE, "" },
    { "/" N_("Match control") "/" N_("End tournament"), "<control><shift>e",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_END, "" },
    { "/" N_("Match control") "/" N_("Step"), "s",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_STEP, "" },
    { "/" N_("Match control") "/" N_("End match"), "<control><shift>m",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_END_MATCH, "" },
    { "/" N_("Match control") "/" N_("Kill marked robot"), "<control><shift>k",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_KILL_MARKED_ROBOT, "" },
    { "/_" N_("Replay"), NULL, NULL, 0, "<Branch>" },
    { "/" N_("Replay") "/tearoff", NULL, NULL, 0, "<Tearoff>" },
    { "/" N_("Replay") "/" N_("Step forward"), "f",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_STEP_FORWARD, "" },
    { "/" N_("Replay") "/" N_("Step backward"), "b",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_STEP_BACKWARD, "" },
    { "/" N_("Replay") "/sep", NULL, NULL, 0, "<Separator>" },
    { "/" N_("Replay") "/" N_("Next match"), "m",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_NEXT_MATCH, "" },
    { "/" N_("Replay") "/" N_("Previous match"), "<shift>m",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_PREV_MATCH, "" },
    { "/" N_("Replay") "/" N_("Next round"), "r",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_NEXT_ROUND, "" },
    { "/" N_("Replay") "/" N_("Previous round"), "<shift>r",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_PREV_ROUND, "" },
    { "/_" N_("Help"), NULL, NULL, 0, "<LastBranch>" },
    { "/" N_("Help") "/tearoff", NULL, NULL, 0, "<Tearoff>" },
    { "/" N_("Help") "/" N_("About"), "<shift>a",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_ABOUT, "" }
  };

  const int nmenu_items = sizeof( menu_items ) / sizeof( menu_items[0] );

  for( int i = 0; i < nmenu_items; i++ )
    menu_items[i].path = translate_menu_path( menu_items[i].path );

  GtkAccelGroup* accel = gtk_accel_group_new();
  item_factory = gtk_item_factory_new( GTK_TYPE_MENU_BAR, "<main>", accel );
  gtk_item_factory_create_items( item_factory, nmenu_items,
                                 menu_items, (gpointer) this );
  gtk_accel_group_attach( accel, GTK_OBJECT( window_p ) );

  GtkWidget* menubar = gtk_item_factory_get_widget( item_factory, "<main>" );

  gtk_box_pack_start( GTK_BOX( main_vbox ), menubar, FALSE, TRUE, 0 );
  gtk_menu_bar_set_shadow_type( GTK_MENU_BAR( menubar ), GTK_SHADOW_OUT );
  gtk_widget_show( menubar );

  show_arena_menu_item = gtk_item_factory_get_widget
    ( item_factory, translate_menu_path("<main>/Windows/Show arena window") );
  show_message_menu_item = gtk_item_factory_get_widget
    ( item_factory, translate_menu_path("<main>/Windows/Show message window") );
  show_score_menu_item = gtk_item_factory_get_widget
    ( item_factory, translate_menu_path("<main>/Windows/Show score window") );

  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( show_arena_menu_item ), TRUE );
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( show_message_menu_item ), TRUE );
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( show_score_menu_item ), TRUE );

  // Blue style

  GtkStyle* status_style = gtk_rc_get_style(window_p);
  if( status_style == NULL )
    status_style = gtk_style_new();
  else
    status_style = gtk_style_copy(status_style);
  // TODO: Make use of old font information or use an option for large fonts
//    GdkFont* temp_font =
//      gdk_font_load( "-*-helvetica-bold-r-normal--*-140-*-*-*-*-*-*" );
//    if( temp_font )
//      status_style->font = temp_font;
  status_style->fg[GTK_STATE_NORMAL] = make_gdk_colour( 0x1111ee );

  // Info frame

  GtkWidget* frame = gtk_frame_new( NULL );
  gtk_container_border_width( GTK_CONTAINER( frame ), 2 );
  gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_ETCHED_IN );
  gtk_box_pack_start( GTK_BOX( main_vbox ), frame, FALSE, TRUE, 0 );
  gtk_widget_show( frame );

  GtkWidget* frame_vbox = gtk_vbox_new( FALSE, 0 );
  gtk_container_border_width( GTK_CONTAINER( frame_vbox ), 3 );
  gtk_container_add( GTK_CONTAINER( frame ), frame_vbox );
  gtk_widget_show( frame_vbox );

  GtkWidget* hbox = gtk_hbox_new ( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( frame_vbox ), hbox, FALSE, TRUE, 0 );
  gtk_widget_show( hbox );

  GtkWidget* label_vbox = gtk_vbox_new ( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), label_vbox, TRUE, TRUE, 0 );
  gtk_widget_show( label_vbox );

  vector<int> widths;
  vector<int> heights;
  for( int i = NO_STATE; i <= EXITING; i++ )
    {
      string infotext = get_status_string( state_t(i) );
      widths.push_back( gdk_string_width( status_style->font, infotext.c_str() ) );
      heights.push_back( gdk_string_height( status_style->font, infotext.c_str() ) );
    }

  int max_label_width  = *(max_element( widths.begin() , widths.end()  )) + 4;
  int max_label_height = *(max_element( heights.begin(), heights.end() )) + 4;

  status_label = gtk_label_new("");
  gtk_widget_set_usize( status_label, max_label_width, max_label_height );
  gtk_widget_set_style( status_label, status_style );
  gtk_label_set_justify( GTK_LABEL( status_label ), GTK_JUSTIFY_CENTER );
  gtk_box_pack_start( GTK_BOX( label_vbox ), status_label, TRUE, TRUE, 0 );
  gtk_widget_show( status_label );
  
  set_status( NO_STATE );

  string infotext = get_matchinfo_string( 100000, 1000, 1000 );
  max_label_width  = gdk_string_width( status_style->font, infotext.c_str() ) + 4;
  max_label_height = gdk_string_height( status_style->font, infotext.c_str() ) + 4;
  matchinfo_label = gtk_label_new("");
  gtk_widget_set_style( matchinfo_label, status_style );
  gtk_widget_set_usize( matchinfo_label, max_label_width, max_label_height );
  gtk_label_set_justify( GTK_LABEL( matchinfo_label ), GTK_JUSTIFY_CENTER );
  gtk_box_pack_start( GTK_BOX( label_vbox ), matchinfo_label, TRUE, TRUE, 0 );
  gtk_widget_show( matchinfo_label );

  set_matchinfo( 0, 0, 0 );

  GtkWidget* debug_level_vbox = gtk_vbox_new ( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), debug_level_vbox, FALSE, FALSE, 0 );
  gtk_widget_show( debug_level_vbox );

  GtkWidget* debug_level_label = gtk_label_new( _("Debug level") );
  gtk_box_pack_start( GTK_BOX( debug_level_vbox ),
                      debug_level_label, TRUE, FALSE, 0 );
  gtk_widget_show( debug_level_label );

  GtkAdjustment* adj =
    (GtkAdjustment*) gtk_adjustment_new( the_gui.get_debug_level(), 0,
                                         max_debug_level, 1, 1, 0 );

  debug_level = gtk_spin_button_new( adj, 0, 0 );
  gtk_signal_connect( GTK_OBJECT( adj ), "value_changed",
                      (GtkSignalFunc) change_debug_level,
                      (gpointer) this );
  gtk_box_pack_start( GTK_BOX( debug_level_vbox ), debug_level, TRUE, FALSE, 0 );
  gtk_widget_show( debug_level );

  gtk_widget_show( window_p );

  hbox = gtk_hbox_new ( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( frame_vbox ), hbox, FALSE, TRUE, 0 );
  gtk_widget_show( hbox );

  char* rew_xpm[13] =
  { "18 10 2 1",
    "       c None",
    "x      c #000000000000",
    "       xx       xx",
    "     xxxx     xxxx",  
    "   xxxxxx   xxxxxx",
    " xxxxxxxx xxxxxxxx",
    "xxxxxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxxxxx",
    " xxxxxxxx xxxxxxxx",
    "   xxxxxx   xxxxxx",
    "     xxxx     xxxx",
    "       xx       xx" };
  char* ffw_xpm[13] =
  { "18 10 2 1",
    "       c None",
    "x      c #000000000000",
    "xx       xx       ",
    "xxxx     xxxx     ",  
    "xxxxxx   xxxxxx   ",
    "xxxxxxxx xxxxxxxx ",
    "xxxxxxxxxxxxxxxxxx",
    "xxxxxxxxxxxxxxxxxx",
    "xxxxxxxx xxxxxxxx ",
    "xxxxxx   xxxxxx   ",
    "xxxx     xxxx     ",
    "xx       xx       " };

  GtkWidget* vbox3 = gtk_vbox_new ( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), vbox3, FALSE, FALSE, 2 );
  gtk_widget_show( vbox3 );

  rewind_button = new_button_from_xpm_d( rew_xpm, 32, 19 );
  gtk_box_pack_start( GTK_BOX( vbox3 ), rewind_button, TRUE, FALSE, 0 );
  gtk_signal_connect( GTK_OBJECT( rewind_button ), "pressed",
                      (GtkSignalFunc) ControlWindow::rewind_pressed,
                      (gpointer) this );
  gtk_signal_connect( GTK_OBJECT( rewind_button ), "released",
                      (GtkSignalFunc) ControlWindow::rewind_released,
                      (gpointer) this );
  gtk_widget_show( rewind_button );

  vbox3 = gtk_vbox_new ( FALSE, 0 );
  gtk_box_pack_end( GTK_BOX( hbox ), vbox3, FALSE, FALSE, 2 );
  gtk_widget_show( vbox3 );

  fast_forward_button = new_button_from_xpm_d( ffw_xpm, 32, 19 );
  gtk_box_pack_start( GTK_BOX( vbox3 ), fast_forward_button, TRUE, FALSE, 0 );
  gtk_signal_connect( GTK_OBJECT( fast_forward_button ), "pressed",
                      (GtkSignalFunc) ControlWindow::fast_forward_pressed,
                      (gpointer) this );
  gtk_signal_connect( GTK_OBJECT( fast_forward_button ), "released",
                      (GtkSignalFunc) ControlWindow::fast_forward_released,
                      (gpointer) this );
  gtk_widget_show( fast_forward_button );

  // TODO: Get length of match (use instead of 120.0)
  current_replay_time_adjustment =
    (GtkAdjustment*) gtk_adjustment_new ( 0.0, 0.0, 120.0, 0.1, 1.0, 1.0 );

  gtk_signal_connect( GTK_OBJECT( current_replay_time_adjustment ), "value_changed",
                      (GtkSignalFunc) change_current_replay_time,
                      (gpointer) this );

  time_control =
    gtk_hscale_new( GTK_ADJUSTMENT( current_replay_time_adjustment ) );
  gtk_widget_set_usize( GTK_WIDGET( time_control ), 50, 30 );
  gtk_range_set_update_policy( GTK_RANGE( time_control ), GTK_UPDATE_DISCONTINUOUS );
  gtk_scale_set_draw_value( GTK_SCALE( time_control ), FALSE );
  gtk_box_pack_start( GTK_BOX( hbox ), time_control, TRUE, TRUE, 2 );
  gtk_widget_show( time_control );

  set_debug_sensitivity( false );
  set_replay_sensitivity( false );

  filesel = new FileSelector<ControlWindow>( this );
  about_window = NULL;
}

ControlWindow::~ControlWindow()
{
  delete filesel;
  destroy_about();
  gtk_widget_destroy( window_p );
}

GtkWidget*
ControlWindow::new_button_from_xpm_d( char** xpm_data,
                                      const int xsize, const int ysize )
{
  GtkWidget* button_w = gtk_button_new();
  GdkPixmap* pixmap;
  GdkBitmap* bitmap_mask;

  pixmap = gdk_pixmap_create_from_xpm_d( window_p->window,
                                         &bitmap_mask,
                                         &(window_p->style->black),
                                         xpm_data );
  GtkWidget* pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
  gtk_widget_show( pixmap_widget );
  gtk_container_add( GTK_CONTAINER( button_w ), pixmap_widget );
  gtk_widget_set_usize( button_w, xsize, ysize );
  return button_w;
}

// ---------------------------------------------------------------------------
// Translates '/' separated words. Ignores preceding '_' and words surrounded
// by '<' '>'
// ---------------------------------------------------------------------------
char*
ControlWindow::translate_menu_path( const char* pathstr_p )
{
#ifndef ENABLE_NLS
  return pathstr_p;
#else
  vector<string> pathlist;
  pathlist = split_string( string(pathstr_p), pathlist, "/" );
  vector<string>::iterator vi;
  for( vi = pathlist.begin(); vi != pathlist.end(); vi++ )
    {
      if( !((*vi)[0] == '<' && (*vi)[(*vi).length() - 1] == '>' ) )
        if( (*vi)[0] == '_' )
          {
            (*vi).erase( 0, 1 );
            *vi = (string)"_" + _( (*vi).c_str() );
          }
        else
          *vi = _( (*vi).c_str() );
    }
  string new_path;
  for( vi = pathlist.begin(); vi != pathlist.end(); vi++ )
    {
      if( ((*vi)[0] == '<' && (*vi)[(*vi).length() - 1] == '>' ) )
        new_path += *vi;
      else
        new_path += "/" + *vi;
    }
  return copy_to_c_string( new_path ); // Memory-leak: never deleted
#endif
}

void
ControlWindow::menu_set_sensitive( const char* pathstr_p, bool sensitive )
{
  GtkWidget* menu_item =
    gtk_item_factory_get_widget( item_factory, translate_menu_path( pathstr_p ) );
  if( !menu_item )
    {
      the_gui.error( false, (string)"Menu with path " + pathstr_p + " doesn't exist.",
                     "ControlWindow::menus_set_sensitive" );
      return;
    }
  if( GTK_IS_MENU(menu_item) )
    menu_item = gtk_menu_get_attach_widget( GTK_MENU(menu_item) );
  gtk_widget_set_sensitive( menu_item, (gint)sensitive );
}

string
ControlWindow::get_status_string( const state_t& state ) const
{
  string infotext;
  switch( state )
    {
    case NO_STATE:
    case NOT_STARTED:
      infotext = (string)_("No game started");
      break;
    case STARTING_ROBOTS:
      infotext = (string)_("Starting robots");
      break;
    case SHUTTING_DOWN_ROBOTS:
      infotext = (string)_("Shutting down robots");
      break;
    case BEFORE_GAME_START:
    case GAME_IN_PROGRESS:
      infotext = (string)_("Running");
      //                  if( pause_after_next_game )
      //                    infotext = (string)_("Pausing after game");
      break;
    case PAUSING_BETWEEN_GAMES:
    case PAUSED:
      infotext = (string)_("Paused");
      break;
    case EXITING:
      infotext = (string)_("Exiting");
      break;
    case FINISHED:
      infotext = (string)_("Finished");
      break;

    default:
      the_gui.error(true, "Unknown state", "ArenaBase::set_state");
    }
  return infotext;
}

void
ControlWindow::set_status( const state_t& state )
{
  gtk_label_set_text( GTK_LABEL( status_label ),
                      get_status_string( state ).c_str() );
}

string
ControlWindow::get_matchinfo_string( const int& time, const int& match_nr,
                                     const int& matches_per_round )
{
  string matchinfo_str = (string)_("Time") + ": " + int2string( time ) + " " +
    _("Match") + ": " + int2string(match_nr) +
    " (" + int2string(matches_per_round) + ")";
  return matchinfo_str;
}

void
ControlWindow::set_matchinfo( const int& time, const int& match_nr,
                              const int& matches_per_round )
{
  gtk_label_set_text( GTK_LABEL( matchinfo_label ),
                      get_matchinfo_string( time, match_nr,
                                            matches_per_round ).c_str() );
}

void
ControlWindow::set_debug_sensitivity( const bool is_debugging )
{
  menu_set_sensitive("<main>/Match control/Step", is_debugging);
  menu_set_sensitive("<main>/Match control/End match", is_debugging);
  menu_set_sensitive("<main>/Match control/Kill marked robot", is_debugging);
  gtk_widget_set_sensitive( debug_level, (gint)is_debugging );
}

void
ControlWindow::set_replay_sensitivity( const bool is_debugging )
{
  menu_set_sensitive("<main>/Replay", is_debugging);
  menu_set_sensitive("<main>/Replay/Step forward", is_debugging);
  menu_set_sensitive("<main>/Replay/Step backward", is_debugging);
  menu_set_sensitive("<main>/Replay/Next match", is_debugging);
  menu_set_sensitive("<main>/Replay/Previous match", is_debugging);
  menu_set_sensitive("<main>/Replay/Next round", is_debugging);
  menu_set_sensitive("<main>/Replay/Previous round", is_debugging);
  gtk_widget_set_sensitive( rewind_button, (gint)is_debugging );
  gtk_widget_set_sensitive( fast_forward_button, (gint)is_debugging );
  gtk_widget_set_sensitive( time_control, (gint)is_debugging );
}

void
ControlWindow::show_about()
{
  destroy_about();
  // The window

  about_window = gtk_window_new( GTK_WINDOW_DIALOG );
  gtk_window_set_title( GTK_WINDOW( about_window ), "About RealTimeBattle" );
  gtk_widget_set_name( about_window, "RTB About" );
  gtk_window_set_policy( GTK_WINDOW( about_window ), FALSE, FALSE, FALSE );
  gtk_window_position( GTK_WINDOW( about_window ), GTK_WIN_POS_CENTER );
  gtk_container_border_width( GTK_CONTAINER( about_window ), 12 );
  gtk_signal_connect( GTK_OBJECT( about_window ), "delete_event",
                      (GtkSignalFunc) about_callback, (gpointer) NULL );

  // Main box

  GtkWidget* vbox = gtk_vbox_new( FALSE, 0 );
  gtk_container_add( GTK_CONTAINER( about_window ), vbox );
  gtk_widget_show( vbox );

  string about_text[] =
  {
    (string)"sep",
    (string)_("RealTimeBattle is a programming game, in which robots controlled by programs are fighting each other. The goal is to destroy the enemies, using the radar to examine the environment and the cannon to shoot."),
    (string)"sep",
    (string)_("Authors of RealTimeBattle is:\n\n"),
    (string)"   Ragnar Ouchterlony\n" + "   Erik Ouchterlony\n",
    (string)"   " + _("Contact: ") + " rtb@users.sourceforge.net",
    (string)"sep",
    (string)_("If you find a bug, please send it to us. This is best done by visiting the bugtracking system at:\n\n"),
    (string)"   http://www.sourceforge.net/bugs/?group_id=561\n\n",
    (string)_("If you, for some reason, can't use the bugtracking system, please send a bug report by email to the authors."),
    (string)"sep",
    (string)_("Remember to visit the home of RealTimeBattle at:") + "\n\n",
    (string)"   http://realtimebattle.sourceforge.net\n" +
    "   http://www.lysator.liu.se/realtimebattle",
    (string)"sep"
  };

  const int ntexts = sizeof( about_text ) / sizeof( about_text[0] );

  for( int i = 0; i < ntexts; i++ )
    {
      if( about_text[i] == "sep" )
        {
          GtkWidget* separator = gtk_hseparator_new();
          gtk_box_pack_start( GTK_BOX( vbox ), separator, FALSE, FALSE, 6 );
          gtk_widget_show( separator );
        }
      else
        {
          GtkWidget* hbox = gtk_hbox_new( FALSE, 0 );
          gtk_box_pack_start( GTK_BOX( vbox ), hbox, TRUE, TRUE, 0 );
          gtk_widget_show( hbox );
          GtkWidget* label = gtk_label_new( about_text[i].c_str() );
          gtk_label_set_line_wrap( GTK_LABEL( label ), TRUE );
          gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
          gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 0 );
          gtk_widget_show( label );
        }
    }

  GtkWidget* button_w = gtk_button_new_with_label( _("Ok") );
  gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
                      (GtkSignalFunc) about_callback, (gpointer) this );
  gtk_box_pack_start( GTK_BOX( vbox ), button_w, TRUE, TRUE, 0 );
  gtk_widget_show( button_w );

  gtk_widget_show( about_window );
}

void
ControlWindow::destroy_about()
{
  if( about_window != NULL )
    {
      gtk_widget_destroy( about_window );
      about_window = NULL;
    }
}

void
ControlWindow::about_callback( GtkWidget* widget, ControlWindow* cw_p )
{
  cw_p->destroy_about();
}

void
ControlWindow::menu_callback( class ControlWindow* cw_p,
                              guint callback_action, GtkWidget *widget )
{
  switch( (menu_t)callback_action )
    {
    case MENU_QUIT:
      the_gui.quit( true );
      break;
    case MENU_LOGFILE:
      break;
    case MENU_MESSAGEFILE:
      break;
    case MENU_NEW_TOURNAMENT:
      the_gui.open_starttournamentwindow();
      break;
    case MENU_REPLAY_TOURNAMENT:
//    if( the_arena_controller.is_started() &&
//        ( the_arena.get_state() != NOT_STARTED &&
//          the_arena.get_state() != FINISHED ) )
//      {
//        string info_text = _("This action will kill the current tournament.\n"
//                             "Do you want to do that?");
//        list<string> string_list;
//        string_list.push_back( string( _("Yes") ) );
//        string_list.push_back( string( _("No")  ) );
//        Dialog( info_text, string_list,
//                (DialogFunction) ControlWindow::kill_and_open_filesel );
//      }
//      else
//        cw_p->open_replay_filesel();
      break;
    case MENU_PAUSE:
//    if( the_arena_controller.is_started() )
//      the_arena.pause_game_toggle();
      break;
    case MENU_STEP:
//    if( the_arena_controller.is_started() )
//      the_arena.step_paused_game();
      break;
    case MENU_END:
//    if( the_arena_controller.is_started() )
//      if( the_arena.get_state() != NOT_STARTED &&
//          the_arena.get_state() != FINISHED )
//        {
//          string info_text = _("This action will kill the current tournament.\n"
//                               "Do you want to do that?");
//          list<string> string_list;
//          string_list.push_back( string( _("Yes") ) );
//          string_list.push_back( string( _("No")  ) );
//          Dialog( info_text, string_list,
//                  (DialogFunction) ControlWindow::end_tournament );
//        }
//    if( the_arena_controller.is_started() && result == 1 )
//      the_arena.interrupt_tournament();
      break;
    case MENU_OPTIONS:
      the_gui.open_optionswindow();
      break;
    case MENU_STATISTICS:
      the_gui.open_statisticswindow();
      break;
    case MENU_SHOW_ARENA:
      {
        bool active = GTK_CHECK_MENU_ITEM( widget )->active;

        if( the_gui.is_arenawindow_up() )
          {
            if( active )
              the_gui.get_arenawindow_p()->
                show_window( the_gui.get_arenawindow_p()->get_window_p(),
                             the_gui.get_arenawindow_p() );
            else
              the_gui.get_arenawindow_p()->
                hide_window( the_gui.get_arenawindow_p()->get_window_p(),
                             NULL, the_gui.get_arenawindow_p() );
          }
      }
      break;
    case MENU_SHOW_MESSAGES:
      {
        bool active = GTK_CHECK_MENU_ITEM( widget )->active;

        if( the_gui.is_messagewindow_up() )
          {
            if( active )
              the_gui.get_messagewindow_p()->
                show_window( the_gui.get_messagewindow_p()->get_window_p(),
                             the_gui.get_messagewindow_p() );
            else
              the_gui.get_messagewindow_p()->
                hide_window( the_gui.get_messagewindow_p()->get_window_p(),
                             NULL, the_gui.get_messagewindow_p() );
          }
      }
      break;
    case MENU_SHOW_SCORE:
      {
        bool active = GTK_CHECK_MENU_ITEM( widget )->active;

        if( the_gui.is_scorewindow_up() )
          {
            if( active )
              the_gui.get_scorewindow_p()->
                show_window( the_gui.get_scorewindow_p()->get_window_p(),
                             the_gui.get_scorewindow_p() );
            else
              the_gui.get_scorewindow_p()->
                hide_window( the_gui.get_scorewindow_p()->get_window_p(),
                             NULL, the_gui.get_scorewindow_p() );
          }
      }
      break;
    case MENU_END_MATCH:
//    if( the_arena_controller.is_started() )
//      if( the_arena.get_state() != NOT_STARTED &&
//          the_arena.get_state() != FINISHED )
//        the_arena.end_game();
      break;
    case MENU_KILL_MARKED_ROBOT:
//    if( the_arena_controller.is_started() )
//      if(the_arena.get_state() == GAME_IN_PROGRESS || 
//         the_arena.get_state() == PAUSED )
//        {
//          Robot* robotp = the_gui.get_scorewindow_p()->get_selected_robot();
//          if( robotp != NULL )
//            robotp->die();
//        }
      break;
    case MENU_STEP_FORWARD:
  //  cout << "Stepping forward" << endl;
  //  the_arena.step_forward_or_backward( true );  
      break;
    case MENU_STEP_BACKWARD:
  //  cout << "Stepping backward" << endl;
  //  the_arena.step_forward_or_backward( false );
      break;
    case MENU_NEXT_MATCH:
  //  the_arena.change_game( 1, 0 );
      break;
    case MENU_PREV_MATCH:
  //  the_arena.change_game( -1, 0 );
      break;
    case MENU_NEXT_ROUND:
  //  the_arena.change_game( 0, 1 );
      break;
    case MENU_PREV_ROUND:
  //  the_arena.change_game( 0, -1 );
      break;
    case MENU_ABOUT:
      cw_p->show_about();
      break;
    }
}

void
ControlWindow::delete_event_occured( GtkWidget* widget, GdkEvent* event,
                                     class ControlWindow* cw_p )
{
  the_gui.quit( true );
}

void
ControlWindow::quit_rtb( GtkWidget* widget,
                         class ControlWindow* cw_p )
{
  the_gui.quit( true );
}

void
ControlWindow::change_debug_level( GtkAdjustment *adj,
                                   class ControlWindow* cw_p )
{
//    if( the_arena_controller.is_started() )
//      the_arena.set_debug_level
//        ( gtk_spin_button_get_value_as_int
//          ( GTK_SPIN_BUTTON( cw_p->debug_level ) ) );
}

void
ControlWindow::open_replay_filesel()
{
  filesel->bring_up( _("Choose a log file to replay"),
                     &ControlWindow::replay );
}

void
ControlWindow::kill_and_open_filesel( int result )
{
//    if( the_arena_controller.is_started() && result == 1 )
//      {
//        the_arena.interrupt_tournament();
//        the_controlwindow.open_replay_filesel();
//      }
}

bool
ControlWindow::is_arenawindow_checked()
{
  return GTK_CHECK_MENU_ITEM( show_arena_menu_item )->active;
}

bool
ControlWindow::is_messagewindow_checked()
{
  return GTK_CHECK_MENU_ITEM( show_message_menu_item )->active;
}

bool
ControlWindow::is_scorewindow_checked()
{
  return GTK_CHECK_MENU_ITEM( show_score_menu_item )->active;
}

void
ControlWindow::replay( const string& filename )
{
//    if( filename.empty() || filename[filename.length() - 1] == '/' )  
//      return;  // no file is selected

//    the_arena_controller.replay_filename = filename;
//    the_arena_controller.start_replay_arena();
}

void
ControlWindow::rewind_pressed( GtkWidget* widget,
                               class ControlWindow* cw_p )
{
  //  cout << "rewinding ... " << endl;
  //  the_arena.change_speed( false, true );
}

void
ControlWindow::rewind_released( GtkWidget* widget,
                                class ControlWindow* cw_p )
{
  //  cout << "until released" << endl;
  //  the_arena.change_speed( false, false );
}

void
ControlWindow::fast_forward_pressed( GtkWidget* widget,
                                     class ControlWindow* cw_p )
{
  //  cout << "forwarding ... " << endl;
  //  the_arena.change_speed( true, true );
}

void
ControlWindow::fast_forward_released( GtkWidget* widget,
                                      class ControlWindow* cw_p )
{
  //  cout << "until released" << endl;
  //  the_arena.change_speed( true, false );
}

void
ControlWindow::change_current_replay_time( GtkAdjustment *adj,
                                           class ControlWindow* cw_p )
{
  //  the_arena.change_replay_time( adj->value );
}

void
ControlWindow::set_progress_time( const double time )
{
  gtk_adjustment_set_value( current_replay_time_adjustment, time );
}

void
ControlWindow::change_time_limitations()
{
//    if( displayed == REPLAY_WIDGETS )
//      {
//        // Possible memory leak: Do not know how to destroy old adjustment
//        // possibly destroyed by gtk_range_set_adjustment()
//        current_replay_time_adjustment =
//          (GtkAdjustment*) gtk_adjustment_new ( 0.0, 0.0,
//                                                the_arena.get_length_of_current_game(),
//                                                0.1, 1.0, 1.0 );
//        gtk_signal_connect( GTK_OBJECT( current_replay_time_adjustment ), "value_changed",
//                            (GtkSignalFunc) change_current_replay_time,
//                            (gpointer) this );
//        gtk_range_set_adjustment( GTK_RANGE( time_control ),
//                                  current_replay_time_adjustment );
//      }
}
