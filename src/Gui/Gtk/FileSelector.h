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

#ifndef __FILESELECTOR__
#define __FILESELECTOR__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string>

template<class T>
class FileSelector
{
public:
  typedef void (T::*result_func_t)( const string& );
  FileSelector                 ( T* obj, const bool ric = false )
    : result_object(obj), result_if_canceled(ric) { filesel = NULL; }
  ~FileSelector                () { bring_down(); }

  // Returns false if bring_up was unsuccessful
  // (Probably due to that there already was a fileselector)
  bool bring_up                ( const string&, result_func_t );
  void bring_down              ( const bool result_ok = false );

  static void destroy_callback ( GtkWidget* widget, class FileSelector* fs_p )
  { fs_p->bring_down(); }
  static void ok_callback      ( GtkWidget* widget, class FileSelector* fs_p )
  { fs_p->bring_down( true ); }
    
private:

  GtkWidget* filesel;

  T* result_object;
  result_func_t result_func;
  bool result_if_canceled;
};

#include <gtk/gtkfilesel.h>

template<class T>
bool
FileSelector<T>::bring_up( const string& title, result_func_t func )
{
  if( filesel != NULL )
    return false;

  result_func = func;

  filesel = gtk_file_selection_new( title.c_str() );
  gtk_signal_connect( GTK_OBJECT( filesel ), "destroy",
                      (GtkSignalFunc) FileSelector::destroy_callback,
                      (gpointer) this );
  gtk_signal_connect
    ( GTK_OBJECT( GTK_FILE_SELECTION( filesel )->cancel_button ), "clicked",
      (GtkSignalFunc) FileSelector::destroy_callback, (gpointer) this );
  gtk_signal_connect
    ( GTK_OBJECT( GTK_FILE_SELECTION( filesel )->ok_button ), "clicked",
      (GtkSignalFunc) FileSelector::ok_callback, (gpointer) this );
  gtk_widget_show( filesel );
  return true;
}

template<class T>
void
FileSelector<T>::bring_down( const bool result_ok )
{
  if( filesel != NULL )
    {
      if( result_if_canceled || result_ok )
        {
          string result("");
          if( result_ok )
            result = gtk_file_selection_get_filename( GTK_FILE_SELECTION( filesel ) );
          (result_object->*result_func)( result );
        }
      gtk_widget_destroy( filesel );
      filesel = NULL;
    }
}

#endif __FILESELECTOR__
