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

#ifndef __OPTIONS_WINDOW__
#define __OPTIONS_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string>
#include <map>

#include "Option.h"

struct _GtkWidget;
typedef struct _GtkWidget GtkWidget;
struct _GtkEntry;
typedef struct _GtkEntry GtkEntry;
union _GdkEvent;
typedef union _GdkEvent GdkEvent;
typedef void* gpointer;
template<class T> struct option_info_t;

template<class T> class FileSelector;

class OptionsWindow
{
private:

public:
  OptionsWindow                   ( const int default_width  = -1,
                                    const int default_height = -1,
                                    const int default_x_pos  = -1,
                                    const int default_y_pos  = -1 );
  ~OptionsWindow                  ();

  void update_all_gtk_entries     ();
  void set_all_options            ();
  void load_options               ( const string& );
  void save_options               ( const string& );

  static void default_opts        ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );
  static void load                ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );
  static void save                ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );
  static void save_def            ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );
  static void apply               ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );
  static void ok                  ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );
  static void cancel              ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );
  static void delete_event_occured( GtkWidget* widget, GdkEvent* event,
                                    class OptionsWindow* optionswindow_p );

  static void grab_windows        ( GtkWidget* widget,
                                    class OptionsWindow* optionswindow_p );

  static void min_callback        ( GtkWidget* widget, Option* option_p );
  static void def_callback        ( GtkWidget* widget, Option* option_p );
  static void max_callback        ( GtkWidget* widget, Option* option_p );
  static void entry_handler       ( GtkWidget* widget, Option* option_p );

  GtkWidget* get_window_p         () { return window_p; }

private:
  struct page_t
  {
    page_t( const string& l, GtkWidget* dt, GtkWidget* et,
            GtkWidget* bt, const int noo )
      : label(l), description_table(dt), entry_table(et), button_table(bt),
        number_of_options(noo), current_row(0) {}
    string label;
    GtkWidget* description_table;
    GtkWidget* entry_table;
    GtkWidget* button_table;
    int number_of_options;
    int current_row;
  };

  typedef void (OptionsWindow::*process_option_func)( const Option*, GtkEntry* );
  void process_all_options        ( process_option_func );

  // Various process_option_func functions
  void set_option                 ( const Option* option_p, GtkEntry* entry );
  void set_entry_to_value         ( const Option* option_p, GtkEntry* entry );
  void set_entry_to_default       ( const Option* option_p, GtkEntry* entry );

  // Map of options and entries
  map<const Option*,GtkEntry*> list_of_options_and_entries;

  // FileSelector
  FileSelector<OptionsWindow>* get_filesel () { return filesel; }
  FileSelector<OptionsWindow>* filesel;

  // The window pointer
  GtkWidget* window_p;
};

#endif __OPTIONS_WINDOW__
