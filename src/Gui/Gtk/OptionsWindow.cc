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
# include <config.h>
#endif

#include <gtk/gtk.h>
#include <map>
#include <vector>

#include "Gui.h"
#include "FileSelector.h"
#include "IntlDefs.h"
#include "Option.h"
#include "OptionsWindow.h"
#include "String.h"

OptionsWindow::OptionsWindow()
{
  nodes_and_options.clear();
}

OptionsWindow::~OptionsWindow()
{
  destroy();
}

void
OptionsWindow::create( const int&  default_width,
                       const int&  default_height,
                       const int&  default_x_pos,
                       const int&  default_y_pos )
{
  // Check that the window isn't already created

  if( is_window_shown() )
    return;

  // Create the window.

  Window::create( GTK_WINDOW_TOPLEVEL,
                  "Options",
                  -1,
                  -1,
                  default_width,
                  default_height,
                  default_x_pos,
                  default_y_pos );

  set_window_title();
  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  gtk_signal_connect( GTK_OBJECT( window_p ),
                      "delete_event",
                      (GtkSignalFunc) OptionsWindow::close_window,
                      (gpointer) this );

  // Create the main box.

  GtkWidget* main_box = gtk_vbox_new( false, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), main_box );
  gtk_widget_show( main_box );

  // Create the scrolled window that contains the ctree.

  GtkWidget* scrolled_window = gtk_scrolled_window_new( 0, 0 );
  gtk_container_set_border_width( GTK_CONTAINER( scrolled_window ), 5 );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled_window ),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS );
  gtk_box_pack_start( GTK_BOX( main_box ), scrolled_window, true, true, 0 );
  gtk_widget_show( scrolled_window );

  // Create the ctree.

  char* titles[] = { _("Option"), _("Value") };

  ctree = gtk_ctree_new_with_titles( 2, 0, titles );
  gtk_container_add( GTK_CONTAINER( scrolled_window ), ctree );

  gtk_signal_connect( GTK_OBJECT( ctree ),
                      "tree_select_row",
                      (GtkSignalFunc) OptionsWindow::ctree_row_selected,
                      (gpointer) this );

  // Column widths is subject to change.
  gtk_clist_set_column_auto_resize( GTK_CLIST( ctree ), 0, true );
  gtk_clist_set_column_auto_resize( GTK_CLIST( ctree ), 1, true );
  gtk_clist_column_titles_passive( GTK_CLIST( ctree ) );
  gtk_clist_set_selection_mode( GTK_CLIST( ctree ), GTK_SELECTION_BROWSE );
  gtk_ctree_set_line_style( GTK_CTREE( ctree ), GTK_CTREE_LINES_DOTTED );
  gtk_widget_set_usize( ctree, 280, 200 );

  gtk_widget_show( ctree );

  // Create frame.

  GtkWidget* frame = gtk_frame_new( 0 );
  gtk_container_border_width( GTK_CONTAINER( frame ), 2 );
  gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_ETCHED_IN );
  gtk_box_pack_start( GTK_BOX( main_box ), frame, false, true, 0 );
  gtk_widget_show( frame );

  // Create vbox in frame.

  GtkWidget* frame_box = gtk_vbox_new( false, 10 );
  gtk_container_border_width( GTK_CONTAINER( frame_box ), 6 );
  gtk_container_add( GTK_CONTAINER( frame ), frame_box );
  gtk_widget_show( frame_box );
  
  // Create label

  option_label = gtk_label_new( "" );
  gtk_box_pack_start( GTK_BOX( frame_box ), option_label, false, false, 0 );
  gtk_widget_show( option_label );

  // Create entry

  GtkWidget* hbox1 = gtk_hbox_new( false, 10 );
  gtk_box_pack_start( GTK_BOX( frame_box ), hbox1, false, false, 0 );
  gtk_widget_show( hbox1 );

  string label_text = (string)_("Value") + ":";
  GtkWidget* label = gtk_label_new( label_text.c_str() );
  gtk_box_pack_start( GTK_BOX( hbox1 ), label, false, false, 0 );
  gtk_widget_show( label );

  option_entry = gtk_entry_new();
  gtk_signal_connect( GTK_OBJECT( option_entry ),
                      "changed",
                      (GtkSignalFunc) OptionsWindow::entry_handler,
                      (gpointer) this );
  gtk_box_pack_start( GTK_BOX( hbox1 ), option_entry, true, true, 0 );
  gtk_widget_show( option_entry );

  // Create min/def/max buttons

  GtkWidget* hbox2 = gtk_hbox_new( false, 10 );
  gtk_box_pack_start( GTK_BOX( frame_box ), hbox2, false, false, 0 );
  gtk_widget_show( hbox2 );

  {
    struct button_mdm_t { string label; GtkWidget** widget_pp; GtkSignalFunc func; };

    static button_mdm_t buttons[] =
    {
      {
        spaced_string( _("Set to minimum") ),
        &option_min_button,
        (GtkSignalFunc) OptionsWindow::min_callback
      },
      {
        spaced_string( _("Set to default") ),
        &option_def_button,
        (GtkSignalFunc) OptionsWindow::def_callback
      },
      {
        spaced_string( _("Set to maximum") ),
        &option_max_button,
        (GtkSignalFunc) OptionsWindow::max_callback
      },
    };

    static const int no_buttons = sizeof( buttons ) / sizeof( buttons[0] );

    for( int i = 0; i < no_buttons; i++ )
      {
        GtkWidget* button = gtk_button_new_with_label( buttons[i].label.c_str() );
        gtk_signal_connect( GTK_OBJECT( button ),
                            "clicked",
                            buttons[i].func,
                            (gpointer) this );
        gtk_box_pack_start( GTK_BOX( hbox2 ), button, true, true, 0 );
        gtk_widget_show( button );
        *(buttons[i].widget_pp) = button;
      }
  }

  // Create bottom buttons

  GtkWidget* hbox3 = gtk_hbox_new( false, 10 );
  gtk_box_pack_start( GTK_BOX( main_box ), hbox3, false, false, 0 );
  gtk_widget_show( hbox3 );

  {
    struct bottom_buttons_t { string label; GtkSignalFunc func; };

    static bottom_buttons_t buttons[] =
    {
      {
        spaced_string( _("Revert to default") ),
        (GtkSignalFunc) OptionsWindow::revert_to_default
      },
      {
        spaced_string( _("Load options") ),
        (GtkSignalFunc) OptionsWindow::load_callback
      },
      {
        spaced_string( _("Save options") ),
        (GtkSignalFunc) OptionsWindow::save_callback
      },
      {
        spaced_string( _("Save as default") ),
        (GtkSignalFunc) OptionsWindow::save_as_default
      },
      {
        spaced_string( _("Apply") ),
        (GtkSignalFunc) OptionsWindow::apply_callback
      },
      {
        spaced_string( _("Ok") ),
        (GtkSignalFunc) OptionsWindow::ok_callback
      },
      {
        spaced_string( _("Cancel") ),
        (GtkSignalFunc) OptionsWindow::cancel_callback
      }
    };

    static const int no_buttons = sizeof( buttons ) / sizeof( buttons[0] );

    GtkWidget* button = 0;
    for( int i = 0; i < no_buttons; i++ )
      {
        button = gtk_button_new_with_label( buttons[i].label.c_str() );
        gtk_signal_connect( GTK_OBJECT( button ),
                            "clicked",
                            buttons[i].func,
                            (gpointer) this );
        gtk_box_pack_start( GTK_BOX( hbox3 ), button, true, true, 0 );
        gtk_widget_show( button );
      }
  }  

  current_node   = 0;
  current_option = 0;

  change_current_option( 0 );

  // TODO: Add all options available.
  add_new_options( the_gui.get_gtk_opts() );

  gtk_widget_show( window_p );
}

void
OptionsWindow::destroy()
{
  nodes_and_options.clear();
  Window::destroy();
}

void
OptionsWindow::set_window_title()
{
  gtk_window_set_title( GTK_WINDOW( window_p ), _("Options") );
}

void
OptionsWindow::add_new_options( const OptionHandler* opts )
{
  char* root_text[2] = { const_cast<char*>( opts->get_section_name().c_str() ), "" };
  GtkCTreeNode* root_node =  gtk_ctree_insert_node( GTK_CTREE( ctree ),
                                                    0, 0,
                                                    root_text,
                                                    5,
                                                    0, 0, 0, 0,
                                                    false, true );
  const vector<string> group_names = opts->get_group_names();
  vector<string>::const_iterator sci;
  GtkCTreeNode* group_node = 0;
  vector<GtkCTreeNode*> group_node_info;

  for( sci = group_names.begin(); sci != group_names.end(); sci ++ )
    {
      char* group_text[2] = { const_cast<char*>( (*sci).c_str() ), "" };
      group_node = gtk_ctree_insert_node( GTK_CTREE( ctree ),
                                          root_node,
                                          group_node,
                                          group_text,
                                          5,
                                          0, 0, 0, 0,
                                          false, false );
      group_node_info.push_back( group_node );
    }

  const map<string,Option*>& all_options = opts->get_options();
  map<string,Option*>::const_iterator soci;
  vector<GtkCTreeNode*> leaf_info( group_node_info.size(), 0 );

  for( soci = all_options.begin(); soci != all_options.end(); soci++ )
    {
      const int current_group = soci->second->get_group();
      char* leaf_text[2] = 
      {
        const_cast<char*>( soci->second->get_description().c_str() ),
        const_cast<char*>( soci->second->get_string_val().c_str() )
      };
      leaf_info[current_group] =
        gtk_ctree_insert_node( GTK_CTREE( ctree ),
                               group_node_info[current_group],
                               leaf_info[current_group],
                               leaf_text,
                               5,
                               0, 0, 0, 0,
                               true, false );
      nodes_and_options[ leaf_info[current_group] ] = soci->second;
    }
}

void
OptionsWindow::process_all_options( process_option_func the_function )
{
  map<GtkCTreeNode*, const Option*>::iterator goi;
  for( goi = nodes_and_options.begin(); goi != nodes_and_options.end(); goi++ )
    (this->*the_function)( goi->first, goi->second );
}

void
OptionsWindow::set_option( GtkCTreeNode*  node,
                           const Option*  option_p )
{
  char* col1_text;
  int res1 = gtk_ctree_node_get_text( GTK_CTREE( ctree ),
                                      node,
                                      1,
                                      &col1_text );
  if( res1 != 0 )
    {
      string new_val( col1_text );
      string old_val( option_p->get_string_val() );
      if( old_val != new_val )
        ; // TODO: Set the option in some way. e.g. send a OptionChangeRequest
    }
}

void
OptionsWindow::set_node_to_value( GtkCTreeNode*  node,
                                  const Option*  option_p )
{
  gtk_ctree_node_set_text( GTK_CTREE( ctree ),
                           node,
                           1,
                           option_p->get_string_val().c_str() );

  if( node == current_node && option_p == current_option )
    gtk_entry_set_text( GTK_ENTRY( option_entry ),
                        option_p->get_string_val().c_str() );
}

void
OptionsWindow::set_node_to_default( GtkCTreeNode*  node,
                                    const Option*  option_p )
{
  gtk_ctree_node_set_text( GTK_CTREE( ctree ),
                           node,
                           1,
                           option_p->get_string_def().c_str() );

  if( node == current_node && option_p == current_option )
    gtk_entry_set_text( GTK_ENTRY( option_entry ),
                        option_p->get_string_def().c_str() );
}

void
OptionsWindow::set_all_options()
{
  if( !(( the_gui.get_game_mode() == COMPETITION_MODE &&
          ( the_gui.get_state() == NO_STATE ||
            the_gui.get_state() == NOT_STARTED ||
            the_gui.get_state() == FINISHED ) ) ||
        ( the_gui.get_game_mode() != COMPETITION_MODE )) )
    return;

  process_all_options( &OptionsWindow::set_option );

  the_gui.set_colours();
}

void
OptionsWindow::update_all_nodes()
{
  process_all_options( &OptionsWindow::set_node_to_value );
}

void
OptionsWindow::revert_all_options_to_default()
{
  process_all_options( &OptionsWindow::set_node_to_default );
}

void
OptionsWindow::load_options( const string& filename )
{
  if( filename.empty() || filename[filename.length() - 1] == '/' )  
    return;  // no file is selected

  // Read all opts
  update_all_nodes();
}

void
OptionsWindow::save_options( const string& filename )
{
  if( filename.empty() || filename[filename.length() - 1] == '/' )  
    return;  // no file is selected

  // Save all opts
}

void
OptionsWindow::change_current_option( GtkCTreeNode*  node )
{
  bool option_node = false;
  if( node != 0 )
    {
      gboolean is_leaf = false;
      gtk_ctree_get_node_info( GTK_CTREE( ctree ),
                               node,
                               0, 0, 0, 0, 0, 0,
                               &is_leaf,
                               0 );
      option_node = is_leaf;
    }

  if( option_node )
    {
      if( current_option == 0 ||
          current_node   == 0 )
        {
          gtk_widget_set_sensitive( option_label, true );
          gtk_widget_set_sensitive( option_entry, true );
          gtk_widget_set_sensitive( option_def_button, true );
        }

      current_node = node;
      current_option = nodes_and_options[node];

      bool use_min_max = true;
      if( current_option->get_string_min().empty() ||
          current_option->get_string_max().empty() )
        use_min_max = false;
      gtk_widget_set_sensitive( option_min_button, use_min_max );
      gtk_widget_set_sensitive( option_max_button, use_min_max );

      gtk_label_set_text( GTK_LABEL( option_label ),
                          current_option->get_description().c_str() );
      char* col1_text;
      int res1 = gtk_ctree_node_get_text( GTK_CTREE( ctree ),
                                          node,
                                          1,
                                          &col1_text );
      if( res1 != 0 )
        gtk_entry_set_text( GTK_ENTRY( option_entry ),
                            col1_text );
      else
        gtk_entry_set_text( GTK_ENTRY( option_entry ),
                            "" );

    }
  else if( current_option != 0 ||
           current_node != 0   ||
           node == 0 )
    {
      current_option = 0;
      current_node   = 0;
      gtk_widget_set_sensitive( option_label, false );
      gtk_widget_set_sensitive( option_entry, false );
      gtk_widget_set_sensitive( option_min_button, false );
      gtk_widget_set_sensitive( option_def_button, false );
      gtk_widget_set_sensitive( option_max_button, false );
      gtk_label_set_text( GTK_LABEL( option_label ), "" );
      gtk_entry_set_text( GTK_ENTRY( option_entry ), "" );
    }
}

gint
OptionsWindow::close_window( GtkWidget*     widget,
                             GdkEvent*      event,
                             OptionsWindow* object_p )
{
  object_p->destroy();
  return true;
}

void
OptionsWindow::ctree_row_selected( GtkWidget*     widget,
                                   GtkCTreeNode*  row,
                                   gint           column,
                                   OptionsWindow* object_p )
{
  object_p->change_current_option( row );
}

// Button callbacks
void
OptionsWindow::min_callback( GtkWidget*      widget,
                             OptionsWindow*  object_p )
{
  string new_val = object_p->current_option->get_string_min();
  string old_val = gtk_entry_get_text( GTK_ENTRY( object_p->option_entry ) );
  if( new_val != old_val )
    gtk_entry_set_text( GTK_ENTRY( object_p->option_entry ),
                        new_val.c_str() );
}

void
OptionsWindow::def_callback( GtkWidget*      widget,
                             OptionsWindow*  object_p )
{
  string new_val = object_p->current_option->get_string_def();
  string old_val = gtk_entry_get_text( GTK_ENTRY( object_p->option_entry ) );
  if( new_val != old_val )
    gtk_entry_set_text( GTK_ENTRY( object_p->option_entry ),
                        new_val.c_str() );
}

void
OptionsWindow::max_callback( GtkWidget*      widget,
                             OptionsWindow*  object_p )
{
  string new_val = object_p->current_option->get_string_max();
  string old_val = gtk_entry_get_text( GTK_ENTRY( object_p->option_entry ) );
  if( new_val != old_val )
    gtk_entry_set_text( GTK_ENTRY( object_p->option_entry ),
                        new_val.c_str() );
}

void
OptionsWindow::revert_to_default( GtkWidget*      widget,
                                  OptionsWindow*  object_p )
{
  object_p->revert_all_options_to_default();
}

void
OptionsWindow::load_callback( GtkWidget*      widget,
                              OptionsWindow*  object_p )
{
  object_p->the_fileselector.bring_up( _("Choose an options file to load."),
                                       &OptionsWindow::load_options,
                                       object_p );
}

void
OptionsWindow::save_callback( GtkWidget*      widget,
                              OptionsWindow*  object_p )
{
  object_p->the_fileselector.bring_up( _("Please choose where to save the options."),
                                       &OptionsWindow::load_options,
                                       object_p );
}

void
OptionsWindow::save_as_default( GtkWidget*      widget,
                                OptionsWindow*  object_p )
{
  // TODO
  //    the_opts.save_all_options_to_file("",true);
}

void
OptionsWindow::apply_callback( GtkWidget*      widget,
                               OptionsWindow*  object_p )
{
  object_p->set_all_options();
}

void
OptionsWindow::ok_callback( GtkWidget*      widget,
                            OptionsWindow*  object_p )
{
  object_p->set_all_options();
  object_p->destroy();
}

void
OptionsWindow::cancel_callback( GtkWidget*      widget,
                                OptionsWindow*  object_p )
{
  object_p->destroy();
}

void
OptionsWindow::entry_handler( GtkWidget*      widget,
                              OptionsWindow*  object_p )
{
  // If not active: return
  if( object_p->current_option == 0 ||
      object_p->current_node == 0 )
    return;

  // Get the text entered.
  string entry_text = gtk_entry_get_text( GTK_ENTRY( object_p->option_entry ) );
  string old_entry_text = entry_text;
  // Make it a valid string
  object_p->current_option->make_correct_string_val( entry_text );

  // Change the entry (if it did make_correct_string_val() changed it).
  if( old_entry_text != entry_text )
    {
      gtk_entry_set_text( GTK_ENTRY( widget ), entry_text.c_str() );
      gtk_signal_emit_stop_by_name( GTK_OBJECT( widget ), "changed" );
    }

  // Update the current node.
  gtk_ctree_node_set_text( GTK_CTREE( object_p->ctree ),
                           object_p->current_node,
                           1,
                           entry_text.c_str() );
}
