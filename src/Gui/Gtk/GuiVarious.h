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

#ifndef __GUIVARIOUS__
#define __GUIVARIOUS__

#include <glib.h>
#include <gdk/gdktypes.h>
#include <gtk/gtkwidget.h>

GdkColor make_gdk_colour(const long col);
int long gdk2hex_colour(const GdkColor& col);

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
gint int_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);
gint float_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);
gint string_case_sensitive_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);
gint string_case_insensitive_compare(GtkCList *clist, gconstpointer row1, gconstpointer row2);
#endif

#endif __GUIVARIOUS__
