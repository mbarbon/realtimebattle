/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef RTB_GTKGUI__OPTIONS_WINDOW__
#define RTB_GTKGUI__OPTIONS_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gtk/gtk.h>

#include <map>

#include "FileSelector.h"
#include "Option.h"
#include "OptionHandler.h"
#include "Window.h"

class OptionsWindow : public Window
{
public:
  OptionsWindow                       ();
  ~OptionsWindow                      ();

  void create                         ( const int&           default_width  = -1,
                                        const int&           default_height = -1,
                                        const int&           default_x_pos  = -1,
                                        const int&           default_y_pos  = -1 );

  void add_new_options                ( const OptionHandler* opts );

  void destroy                        ();

  void set_window_title               ();

private:

  // Function to use when processing all options.
  typedef void (OptionsWindow::*process_option_func)( GtkCTreeNode*, const Option* );
  void process_all_options            ( process_option_func );
  // Functions used as arguments to process_all_options().
  void set_option                     ( GtkCTreeNode*        node,
                                        const Option*        option_p );
  void set_node_to_value              ( GtkCTreeNode*        node,
                                        const Option*        option_p );
  void set_node_to_default            ( GtkCTreeNode*        node,
                                        const Option*        option_p );

  // Functions that uses process_all_options().
  void set_all_options                ();
  void update_all_nodes               ();
  void revert_all_options_to_default  ();

  // Load/Save
  void load_options                   ( const string&        filename );
  void save_options                   ( const string&        filename );

  // Change the option that is current.
  void change_current_option          ( GtkCTreeNode*        node );

  // Graphics callbacks
  static gint close_window            ( GtkWidget*           widget,
                                        GdkEvent*            event,
                                        OptionsWindow*       object_p );
  static void ctree_row_selected      ( GtkWidget*           widget,
                                        GtkCTreeNode*        row,
                                        gint                 column,
                                        OptionsWindow*       object_p );
  static void min_callback            ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void def_callback            ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void max_callback            ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void revert_to_default       ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void load_callback           ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void save_callback           ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void save_as_default         ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void apply_callback          ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void ok_callback             ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void cancel_callback         ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  static void entry_handler           ( GtkWidget*           widget,
                                        OptionsWindow*       object_p );
  
  // Variables
  GtkWidget*                          ctree;
  GtkWidget*                          option_label;
  GtkWidget*                          option_entry;
  GtkWidget*                          option_min_button;
  GtkWidget*                          option_def_button;
  GtkWidget*                          option_max_button;

  map<GtkCTreeNode*, const Option*>   nodes_and_options;
  GtkCTreeNode*                       current_node;
  const Option*                       current_option;

  FileSelector<OptionsWindow>         the_fileselector;
};

#endif // RTB_GTKGUI__OPTIONS_WINDOW__
