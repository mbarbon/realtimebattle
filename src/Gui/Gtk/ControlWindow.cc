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

  window_hbox = gtk_hbox_new ( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), window_hbox );
  gtk_widget_show( window_hbox );

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_hbox ), vbox );
  gtk_widget_show( vbox );

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
    { "/_" N_("File"), NULL, NULL, 0, "<Branch>" },
    { "/"  N_("File") "/" N_("Quit"), "<control>q",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_QUIT, "" },
    { "/_" N_("Tournament"), NULL, NULL, 0, "<Branch>" },
    { "/"  N_("Tournament") "/" N_("New tournament"), "<control>n",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_NEW_TOURNAMENT, "" },
    { "/" N_("Tournament") "/" N_("Replay tournament"), "<control>r",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_REPLAY_TOURNAMENT, "" },
    { "/" N_("Tournament") "/" N_("Pause"), "<shift>p",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_PAUSE, "" },
    { "/" N_("Tournament") "/" N_("End"), "<control>e",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_END, "" },
    { "/_" N_("Windows"), NULL, NULL, 0, "<Branch>" },
    { "/" N_("Windows") "/" N_("Options"), "<shift>o",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_OPTIONS, "" },
    { "/" N_("Windows") "/" N_("Statistics"), "<shift>s",
      (GtkItemFactoryCallback) ControlWindow::menu_callback, MENU_STATISTICS, "" },
    { "/" N_("Windows") "/sep", NULL, NULL, 0, "<Separator>" },
    { "/" N_("Windows") "/" N_("Show arena window"), "<control>a",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_SHOW_ARENA, "<CheckItem>" },
    { "/"  N_("Windows") "/" N_("Show message window"), "<control>m",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_SHOW_MESSAGES, "<CheckItem>" },
    { "/"  N_("Windows") "/" N_("Show score window"), "<control>s",
      (GtkItemFactoryCallback) ControlWindow::menu_callback,
      MENU_SHOW_SCORE, "<CheckItem>" }
  };

  const int nmenu_items = sizeof( menu_items ) / sizeof( menu_items[0] );

  for( int i = 0; i < nmenu_items; i++ )
    menu_items[i].path = translate_menu_path( menu_items[i].path );

  GtkAccelGroup* accel = gtk_accel_group_new();
  GtkItemFactory* item_factory = 
    gtk_item_factory_new( GTK_TYPE_MENU_BAR, "<main>", accel );
  gtk_item_factory_create_items( item_factory, nmenu_items,
                                 menu_items, (gpointer) this );
  gtk_accel_group_attach( accel, GTK_OBJECT( window_p ) );

  GtkWidget* menubar = gtk_item_factory_get_widget( item_factory, "<main>" );

  gtk_box_pack_start( GTK_BOX( vbox ), menubar, FALSE, TRUE, 0 );
  gtk_menu_bar_set_shadow_type( GTK_MENU_BAR( menubar ), GTK_SHADOW_OUT );
  gtk_widget_show( menubar );

  show_arena_menu_item = gtk_item_factory_get_widget
    ( item_factory, translate_menu_path("<main>/Windows/Show arena window") );
  show_message_menu_item = gtk_item_factory_get_widget
    ( item_factory, translate_menu_path("<main>/Windows/Show message window") );
  show_score_menu_item = gtk_item_factory_get_widget
    ( item_factory, translate_menu_path("<main>/Windows/Show score window") );

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( show_arena_menu_item ), TRUE );
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( show_message_menu_item ), TRUE );
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( show_score_menu_item ), TRUE );
#else
  gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM( show_arena_menu_item ), TRUE );
  gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM( show_message_menu_item ), TRUE );
  gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM( show_score_menu_item ), TRUE );
#endif

  GtkStyle* status_style = gtk_rc_get_style(window_p);
  if( status_style == NULL )
    status_style = gtk_style_new();
  else
    status_style = gtk_style_copy(status_style);
  GdkFont* temp_font =
    gdk_font_load( "-*-helvetica-bold-r-normal--17-*-*-*-*-*-*-*" );
  if( temp_font )
    status_style->font = temp_font;
  status_style->fg[GTK_STATE_NORMAL] = make_gdk_colour( 0x1111ee );
  

  GtkWidget* frame = gtk_frame_new("Game status");
  gtk_container_border_width( GTK_CONTAINER( frame ), 2 );
  gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_ETCHED_IN );
  gtk_box_pack_start( GTK_BOX( vbox ), frame, FALSE, TRUE, 0 );
  gtk_widget_show( frame );

  GtkWidget* vbox2 = gtk_vbox_new( FALSE, 0 );
  gtk_container_border_width( GTK_CONTAINER( vbox2 ), 5 );
  gtk_container_add( GTK_CONTAINER( frame ), vbox2 );
  gtk_widget_show( vbox2 );

  status_label = gtk_label_new("");
  gtk_widget_set_style( status_label, status_style );
  gtk_label_set_justify( GTK_LABEL( status_label ), GTK_JUSTIFY_CENTER );
  gtk_box_pack_start( GTK_BOX( vbox2 ), status_label, TRUE, TRUE, 0 );
  gtk_widget_show( status_label );
  
  set_status( NO_STATE );

  vseparator = NULL;
  extra_vbox = NULL;
  filesel = new FileSelector<ControlWindow>( this );

  remove_replay_widgets();

  gtk_widget_show( window_p );
}

ControlWindow::~ControlWindow()
{
  gtk_widget_destroy( window_p );
}

void
ControlWindow::remove_replay_widgets()
{
//    if( the_arena_controller.game_mode == DEBUG_MODE )
//      display_debug_widgets();
//    else
//      clear_extra_widgets();
}

void
ControlWindow::clear_extra_widgets()
{
  if( extra_vbox != NULL ) gtk_widget_destroy( extra_vbox );
  if( vseparator != NULL ) gtk_widget_destroy( vseparator );

  extra_vbox = NULL;
  vseparator = NULL;

  displayed = NO_WIDGETS;
}

void
ControlWindow::display_debug_widgets()
{
  clear_extra_widgets();
  
  vseparator = gtk_vseparator_new();
  gtk_box_pack_start( GTK_BOX (window_hbox), vseparator, FALSE, FALSE, 0 );
  gtk_widget_show( vseparator );

  extra_vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_hbox ), extra_vbox );
  gtk_widget_show( extra_vbox );

  struct button_t { string label; GtkSignalFunc func; int pack; };
  struct button_t debug_buttons[] = {
    { (string)_(" Step "), 
      (GtkSignalFunc) ControlWindow::step      , TRUE  },
    { (string)_(" End Game "), 
      (GtkSignalFunc) ControlWindow::end_game  , TRUE  },
    { (string)_(" Kill Marked Robot "), 
      (GtkSignalFunc) ControlWindow::kill_robot, TRUE  } };

  GtkWidget* button_hbox = NULL;
  for(int i = 0;i < 3; i++)
    {
      if( i == 0 || i == 2 )
        {
          button_hbox = gtk_hbox_new( FALSE, 10 );
          gtk_box_pack_start( GTK_BOX( extra_vbox ), button_hbox,
                              FALSE, FALSE, 0);
          gtk_widget_show( button_hbox );
        }
      GtkWidget* button = 
        gtk_button_new_with_label( debug_buttons[i].label.c_str() );
      gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                          (GtkSignalFunc) debug_buttons[i].func,
                          (gpointer) NULL );
      gtk_box_pack_start( GTK_BOX( button_hbox ), button,
                          TRUE, debug_buttons[i].pack , 0);
      gtk_widget_show( button );
    }

  button_hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX( extra_vbox ), button_hbox,
                      FALSE, FALSE, 0);
  gtk_widget_show( button_hbox );

  GtkWidget* label = gtk_label_new( _(" Debug Level: ") );
  gtk_box_pack_start( GTK_BOX( button_hbox ), label, TRUE, FALSE, 0 );
  gtk_widget_show( label );

  GtkAdjustment* adj =
    (GtkAdjustment*) gtk_adjustment_new( the_gui.get_debug_level(), 0,
                                         max_debug_level, 1, 1, 0 );

  debug_level = gtk_spin_button_new( adj, 0, 0 );
  gtk_signal_connect( GTK_OBJECT( adj ), "value_changed",
                      (GtkSignalFunc) change_debug_level,
                      (gpointer) this );
  gtk_box_pack_start( GTK_BOX( button_hbox ), debug_level, TRUE, FALSE, 0 );
  gtk_widget_show( debug_level );

  displayed = DEBUG_WIDGETS;
}

void
ControlWindow::display_replay_widgets()
{
  clear_extra_widgets();

  vseparator = gtk_vseparator_new();
  gtk_box_pack_start( GTK_BOX (window_hbox), vseparator, FALSE, FALSE, 0 );
  gtk_widget_show( vseparator );

  extra_vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_hbox ), extra_vbox );
  gtk_widget_show( extra_vbox );

  GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX( extra_vbox ), hbox, FALSE, FALSE, 0 );
  gtk_widget_show( hbox );

//    current_replay_time_adjustment =
//      (GtkAdjustment*) gtk_adjustment_new ( 0.0, 0.0,
//                                            the_arena.get_length_of_current_game(),
//                                            0.1, 1.0, 1.0 );

  gtk_signal_connect( GTK_OBJECT( current_replay_time_adjustment ), "value_changed",
                      (GtkSignalFunc) change_current_replay_time,
                      (gpointer) this );

  time_control =
    gtk_hscale_new( GTK_ADJUSTMENT( current_replay_time_adjustment ) );
  gtk_widget_set_usize( GTK_WIDGET( time_control ), 150, 30 );
  gtk_range_set_update_policy( GTK_RANGE( time_control ), GTK_UPDATE_DELAYED );
  gtk_scale_set_value_pos( GTK_SCALE( time_control ), GTK_POS_TOP);
  gtk_scale_set_digits( GTK_SCALE( time_control ), 0 );
  gtk_scale_set_draw_value( GTK_SCALE( time_control ), TRUE );
  gtk_box_pack_start( GTK_BOX( hbox ), time_control, TRUE, TRUE, 0 );
  gtk_widget_show( time_control );

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

  struct button_t
  {
    char** xpm;
    string label;
    GtkSignalFunc clicked_func;
    GtkSignalFunc pressed_func;
    GtkSignalFunc released_func;
  };

  struct button_t replay_buttons[] = {
    { rew_xpm, (string)"",
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::rewind_pressed,
      (GtkSignalFunc) ControlWindow::rewind_released },
    { ffw_xpm, (string)"", 
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::fast_forward_pressed,
      (GtkSignalFunc) ControlWindow::fast_forward_released },
    { NULL, (string)_(" Step forward "), 
      (GtkSignalFunc) ControlWindow::step_forward,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (string)_(" Step backward "), 
      (GtkSignalFunc) ControlWindow::step_backward,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (string)_(" Next Game "), 
      (GtkSignalFunc) ControlWindow::next_game,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (string)_(" Prev Game "), 
      (GtkSignalFunc) ControlWindow::prev_game,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (string)_(" Next Seq "), 
      (GtkSignalFunc) ControlWindow::next_seq,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy },
    { NULL, (string)_(" Prev Seq "), 
      (GtkSignalFunc) ControlWindow::prev_seq,
      (GtkSignalFunc) ControlWindow::dummy,
      (GtkSignalFunc) ControlWindow::dummy } };

  GtkWidget* button_hbox = NULL;

  for(int i = 0;i < 8; i++)
    {
      if( i == 0 || i == 4 )
        {
          button_hbox = gtk_hbox_new( FALSE, 10 );
          gtk_box_pack_start( GTK_BOX( extra_vbox ), button_hbox,
                              FALSE, FALSE, 0);
          gtk_widget_show( button_hbox );
        }
      GtkWidget* button_w = NULL;
      if( replay_buttons[i].xpm != NULL )
        {
          button_w = gtk_button_new();
          GdkPixmap* pixmap;
          GdkBitmap* bitmap_mask;

          pixmap = gdk_pixmap_create_from_xpm_d( window_p->window,
                                                 &bitmap_mask,
                                                 &(window_p->style->black),
                                                 replay_buttons[i].xpm );
          GtkWidget* pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
          gtk_widget_show( pixmap_widget );
          gtk_container_add( GTK_CONTAINER( button_w ), pixmap_widget );
          gtk_widget_set_usize( button_w, 32, 20 );
        }
      else
        button_w = 
          gtk_button_new_with_label( replay_buttons[i].label.c_str() );

      gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
                          (GtkSignalFunc) replay_buttons[i].clicked_func,
                          (gpointer) NULL );
      gtk_signal_connect( GTK_OBJECT( button_w ), "pressed",
                          (GtkSignalFunc) replay_buttons[i].pressed_func,
                          (gpointer) NULL );
      gtk_signal_connect( GTK_OBJECT( button_w ), "released",
                          (GtkSignalFunc) replay_buttons[i].released_func,
                          (gpointer) NULL );
      gtk_box_pack_start( GTK_BOX( button_hbox ), button_w,
                          TRUE, TRUE , 0);
      gtk_widget_show( button_w );
    }

  displayed = REPLAY_WIDGETS;
}

// ---------------------------------------------------------------------------
// Translates '/' separated words. Ignores preceding '_' and words surrounded
// by '<' '>'
// ---------------------------------------------------------------------------
char*
ControlWindow::translate_menu_path( char* pathstr_p )
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

void
ControlWindow::set_status( const state_t state )
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

  gtk_label_set_text( GTK_LABEL( status_label ), infotext.c_str() );
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
    case MENU_END:
      break;
    case MENU_OPTIONS:
      break;
    case MENU_STATISTICS:
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
ControlWindow::pause( GtkWidget* widget, class ControlWindow* cw_p )
{
}

void
ControlWindow::step( GtkWidget* widget, gpointer data )
{
//    if( the_arena_controller.is_started() )
//      the_arena.step_paused_game();
}

void
ControlWindow::end_game( GtkWidget* widget, gpointer data )
{
//    if( the_arena_controller.is_started() )
//      if( the_arena.get_state() != NOT_STARTED &&
//          the_arena.get_state() != FINISHED )
//        the_arena.end_game();
}

void
ControlWindow::kill_robot( GtkWidget* widget, gpointer data )
{
//    if( the_arena_controller.is_started() )
//      if(the_arena.get_state() == GAME_IN_PROGRESS || 
//         the_arena.get_state() == PAUSED )
//        {
//          Robot* robotp = the_gui.get_scorewindow_p()->get_selected_robot();
//          if( robotp != NULL )
//            robotp->die();
//        }
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
  if( filename.empty() || filename[filename.length() - 1] == '/' )  
    return;  // no file is selected

//    the_arena_controller.replay_filename = filename;
//    the_arena_controller.start_replay_arena();
}

void
ControlWindow::end_clicked( GtkWidget* widget, gpointer data )
{
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
}

void
ControlWindow::end_tournament( int result )
{
//    if( the_arena_controller.is_started() && result == 1 )
//      the_arena.interrupt_tournament();
}

void
ControlWindow::options_clicked( GtkWidget* widget,
                                class ControlWindow* cw_p )
{
  the_gui.open_optionswindow();
}

void
ControlWindow::statistics_clicked( GtkWidget* widget,
                                   class ControlWindow* cw_p )
{
  the_gui.open_statisticswindow();
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
ControlWindow::step_forward( GtkWidget* widget,
                             class ControlWindow* cw_p )
{
  //  cout << "Stepping forward" << endl;
  //  the_arena.step_forward_or_backward( true );  
}

void
ControlWindow::step_backward( GtkWidget* widget,
                              class ControlWindow* cw_p )
{
  //  cout << "Stepping backward" << endl;
  //  the_arena.step_forward_or_backward( false );
}

void
ControlWindow::next_game( GtkWidget* widget,
                          class ControlWindow* cw_p )
{
  //  the_arena.change_game( 1, 0 );
}

void
ControlWindow::prev_game( GtkWidget* widget,
                          class ControlWindow* cw_p )
{
  //  the_arena.change_game( -1, 0 );
}

void
ControlWindow::next_seq( GtkWidget* widget,
                         class ControlWindow* cw_p )
{
  //  the_arena.change_game( 0, 1 );
}

void
ControlWindow::prev_seq( GtkWidget* widget,
                         class ControlWindow* cw_p )
{
  //  the_arena.change_game( 0, -1 );
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
