/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <iostream.h>
#include <iomanip.h>
#include <math.h>
//#include <string.h>
#include <stdlib.h>

#include "Gui.h"
#include "Arena.h"
#include "MovingObject.h"
#include "Various.h"

void
statistics_button_callback(GtkWidget *widget, gpointer data)
{
  if(the_arena.get_state() != NOT_STARTED)
    {
      if(the_gui.get_statistics_up() == false)
        the_gui.setup_statistics_window();
      else
        the_gui.close_statistics_window();
    }
}

void
save_statistics_callback(GtkWidget *widget, gpointer data)
{
  if(the_gui.get_filesel_widget() == NULL)
    {
      GtkWidget* filesel = gtk_file_selection_new( "Choose an statistics file" );
      gtk_signal_connect (GTK_OBJECT (filesel), "destroy",
                          (GtkSignalFunc) destroy_stats_filesel, GTK_OBJECT(filesel));
      gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->ok_button),
                          "clicked", (GtkSignalFunc) save_stats_filesel_ok_selected, filesel );
      gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->cancel_button),
                                 "clicked", (GtkSignalFunc) destroy_stats_filesel,
                                 GTK_OBJECT (filesel));
      gtk_widget_show(filesel);
      the_gui.set_filesel_widget(filesel);
    }
}

void
save_stats_filesel_ok_selected (GtkWidget * widget, GtkFileSelection * fs)
{
  the_arena.save_statistics_to_file(gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
  destroy_stats_filesel();
}

void
destroy_stats_filesel()
{
  gtk_widget_destroy(the_gui.get_filesel_widget());
  the_gui.set_filesel_widget(NULL);
}

void 
stat_clist_column_callback (GtkCList *clist, gint column, gpointer data)
{
  the_gui.change_sorting_in_clist(column);
}

void
buttons_in_statistics_callback(GtkWidget *widget, gpointer type_p)
{
  switch(*(int *)type_p)
    {
    case STAT_BUTTON_TOTAL:
      the_gui.change_stat_type( STAT_TABLE_TOTAL );
      break;
    case STAT_BUTTON_SEQUENCE:
      the_gui.change_stat_type( STAT_TABLE_SEQUENCE );
      break;
    case STAT_BUTTON_GAME:
      the_gui.change_stat_type( STAT_TABLE_GAME );
      break;
    case STAT_BUTTON_ROBOT:
      the_gui.change_stat_type( STAT_TABLE_ROBOT );
      break;
    case STAT_BUTTON_FIRST:
      the_gui.change_statistics( -1, true );
      break;
    case STAT_BUTTON_PREV:
      the_gui.change_statistics( -1, false );
      break;
    case STAT_BUTTON_UPDATE:
      the_gui.add_the_statistics_to_clist();
      break;
    case STAT_BUTTON_NEXT:
      the_gui.change_statistics( 1, false );
      break;
    case STAT_BUTTON_LAST:
      the_gui.change_statistics( 1, true );
      break;
    default:
      break;
    }
}

void
Gui::change_stat_type( stat_table_t type )
{
  gtk_clist_freeze(GTK_CLIST(stat_clist));
  if(type == STAT_TABLE_TOTAL || type == STAT_TABLE_SEQUENCE)
    {
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
      gtk_clist_set_column_visibility (GTK_CLIST(stat_clist),4,TRUE);
#else
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 4, "Games ");
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 4, 45);
#endif
    }
  else
    {
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
      gtk_clist_set_column_visibility (GTK_CLIST(stat_clist),4,FALSE);
#else
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 4, "");
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 4, 0);
#endif
    }
  if(stat_table_type == STAT_TABLE_ROBOT && type != STAT_TABLE_ROBOT)
    {
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 0, "");
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 1, "Name");
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 0, 20);
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 1, 120);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_CENTER);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_LEFT);
    }
  if(stat_table_type != STAT_TABLE_ROBOT && type == STAT_TABLE_ROBOT)
    {
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 0, "Seq  ");
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 1, "Game  ");
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 0, 40);
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 1, 45);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_RIGHT);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_RIGHT);
    }
  stat_table_type = type;
  switch( type )
    {
    case STAT_TABLE_TOTAL:
      change_sorting_in_clist( 6 );
      break;
    case STAT_TABLE_SEQUENCE:
      change_sorting_in_clist( 6 );
      stat_looking_at_nr = the_arena.get_sequence_nr();
      break;
    case STAT_TABLE_GAME:
      change_sorting_in_clist( 3 );
      stat_looking_at_nr = ( the_arena.get_sequence_nr() - 1 ) * the_arena.get_games_per_sequence()
        + the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence();
      break;
    case STAT_TABLE_ROBOT:
      change_sorting_in_clist( 0 );
      stat_looking_at_nr = 1;
      break;
    }
  add_the_statistics_to_clist();
  gtk_clist_thaw(GTK_CLIST(stat_clist));
}

void
Gui::change_sorting_in_clist( const int column )
{
  gtk_clist_freeze(GTK_CLIST(stat_clist));
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  switch(column)
    {
    case 0:
      if(stat_table_type == STAT_TABLE_ROBOT)
        {
          gtk_clist_set_compare_func(GTK_CLIST(stat_clist),float_compare);
          gtk_clist_set_sort_column(GTK_CLIST(stat_clist),column);
          gtk_clist_set_sort_type(GTK_CLIST(stat_clist),GTK_SORT_ASCENDING);
          gtk_clist_sort(GTK_CLIST(stat_clist));
        }
      break;
    case 1:
      if(stat_table_type != STAT_TABLE_ROBOT)
        {
          gtk_clist_set_compare_func(GTK_CLIST(stat_clist),string_case_insensitive_compare);
          gtk_clist_set_sort_column(GTK_CLIST(stat_clist),column);
          gtk_clist_set_sort_type(GTK_CLIST(stat_clist),GTK_SORT_ASCENDING);
          gtk_clist_sort(GTK_CLIST(stat_clist));
        }
      else
        {
          gtk_clist_set_compare_func(GTK_CLIST(stat_clist),float_compare);
          gtk_clist_set_sort_column(GTK_CLIST(stat_clist),column);
          gtk_clist_set_sort_type(GTK_CLIST(stat_clist),GTK_SORT_ASCENDING);
          gtk_clist_sort(GTK_CLIST(stat_clist));
        }
      break;
    case 2:
      gtk_clist_set_compare_func(GTK_CLIST(stat_clist),float_compare);
      gtk_clist_set_sort_column(GTK_CLIST(stat_clist),column);
      gtk_clist_set_sort_type(GTK_CLIST(stat_clist),GTK_SORT_ASCENDING);
      gtk_clist_sort(GTK_CLIST(stat_clist));
      break;
    case 3:
    case 5:
    case 6:
      gtk_clist_set_compare_func(GTK_CLIST(stat_clist),float_compare);
      gtk_clist_set_sort_column(GTK_CLIST(stat_clist),column);
      gtk_clist_set_sort_type(GTK_CLIST(stat_clist),GTK_SORT_DESCENDING);
      gtk_clist_sort(GTK_CLIST(stat_clist));
      break;
    case 4:
      if(stat_table_type == STAT_TABLE_TOTAL || stat_table_type == STAT_TABLE_SEQUENCE)
        {
          gtk_clist_set_compare_func(GTK_CLIST(stat_clist),float_compare);
          gtk_clist_set_sort_column(GTK_CLIST(stat_clist),column);
          gtk_clist_set_sort_type(GTK_CLIST(stat_clist),GTK_SORT_ASCENDING);
          gtk_clist_sort(GTK_CLIST(stat_clist));
        }
      break;
    }
#endif
  gtk_clist_thaw(GTK_CLIST(stat_clist));
}

void
Gui::change_statistics( int change, bool absolut_change )
{
  int game_nr = the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence();

  int max_nr = 0;

  switch(stat_table_type)
    {
    case STAT_TABLE_TOTAL:
      max_nr = -1;
      break;
    case STAT_TABLE_SEQUENCE:
      max_nr = the_arena.get_sequence_nr();
      break;
    case STAT_TABLE_GAME:
      max_nr = ( the_arena.get_sequence_nr() - 1 ) * the_arena.get_games_per_sequence() + game_nr;
      break;
    case STAT_TABLE_ROBOT:
      GList* gl;
      for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
        max_nr++;
      break;
    }

  if( absolut_change )
    if( change == -1 )
      stat_looking_at_nr = 1;
    else if (change == 1)
      if( max_nr > 0 )
        stat_looking_at_nr = max_nr;

  if( !absolut_change )
    {
      stat_looking_at_nr += change;
      if(stat_looking_at_nr <= 0)
        stat_looking_at_nr = 1;
      if(stat_looking_at_nr > max_nr)
        stat_looking_at_nr = max_nr;
    }

  add_the_statistics_to_clist();
}

void
Gui::add_new_row( void * rp, void * sp, int games_played )
{
  Robot * robotp = (Robot *)rp;
  stat_t * statp = (stat_t *)sp;
  char * list[8];
              
  for(int j=0;j<8;j++)
    {
      list[j] = new char[30];
      strcpy(list[j],"");
    }

  int row = gtk_clist_append(GTK_CLIST(stat_clist), list);

  for(int i=0; i<8; i++) delete [] list[i];

  gtk_clist_set_foreground(GTK_CLIST(stat_clist), row, the_arena.get_foreground_colour_p());
  gtk_clist_set_background(GTK_CLIST(stat_clist), row, the_arena.get_background_colour_p());

  if(stat_table_type == STAT_TABLE_GAME ||
     stat_table_type == STAT_TABLE_SEQUENCE ||
     stat_table_type == STAT_TABLE_TOTAL)
    {
      GdkPixmap * colour_pixmap;
      GdkBitmap * bitmap_mask;

      robotp->get_stat_pixmap(statistics_window->window, colour_pixmap, bitmap_mask);

      gtk_clist_set_pixmap(GTK_CLIST(stat_clist), row, 0, colour_pixmap, bitmap_mask);
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 1, robotp->get_robot_name().non_const_chars());
    }

  if(stat_table_type == STAT_TABLE_ROBOT)
    {
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 0, String(statp->sequence_nr).non_const_chars());
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 1, String(statp->game_nr).non_const_chars());
    }

  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 2, String(statp->position).non_const_chars());

  String str;
  if(stat_table_type == STAT_TABLE_SEQUENCE ||
     stat_table_type == STAT_TABLE_TOTAL)
    str = String(statp->points, 2, STRING_FIXED_FORM);
  else
    str = String(statp->points);

  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 3, str.non_const_chars());

  if( stat_table_type == STAT_TABLE_TOTAL ||
      stat_table_type == STAT_TABLE_SEQUENCE )
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 4, String(games_played).non_const_chars());
  else
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 4, String("").non_const_chars());

  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 5, String(statp->time_survived, 2, STRING_FIXED_FORM).non_const_chars());
  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 6, String(statp->total_points).non_const_chars());
}

void
Gui::add_the_statistics_to_clist()
{
  GList* gl, * stat_gl;
  Robot* robotp;

  stat_make_title_button();
  gtk_clist_freeze(GTK_CLIST(stat_clist));
  gtk_clist_clear(GTK_CLIST(stat_clist));
  switch( stat_table_type )
    {
    case STAT_TABLE_TOTAL:
      {
        int number_of_robots = 0;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          number_of_robots++;

        double points[number_of_robots];
        int position[number_of_robots];

        int robot_nr = -1;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            robot_nr++;
            robotp = (Robot *)gl->data;

            stat_t * statp = NULL;
            points[robot_nr] = 0;

            for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
              {
                statp = (stat_t*)(stat_gl->data);
                points[robot_nr] += statp->points;
              }
          }

        for(int i = 0;i<number_of_robots;i++)
          {
            int temp_pos = 1;
            for(int j = 0;j<number_of_robots;j++)
              if(points[j] > points[i])
                temp_pos++;
            position[i] = temp_pos;
          }

        robot_nr = -1;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            robot_nr++;
            robotp = (Robot *)gl->data;
            stat_t average_stat(0,0,0,0.0,0.0,0.0);
            stat_t * statp = NULL;
            int number_of_stat_found = 0;

            for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
              {
                statp = (stat_t*)(stat_gl->data);
                number_of_stat_found++;
                average_stat.points += statp->points;
                average_stat.time_survived += statp->time_survived;
                average_stat.total_points += statp->points;
              }
            if( number_of_stat_found > 0 )
              {
                average_stat.position = position[robot_nr];
                average_stat.points /= number_of_stat_found;
                average_stat.time_survived /= number_of_stat_found;
                add_new_row( robotp, &average_stat, number_of_stat_found );
              }

            
          }
        break;
      }
    case STAT_TABLE_SEQUENCE:
      {
        int number_of_robots = 0;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          number_of_robots++;

        double points[number_of_robots];
        int position[number_of_robots];

        int robot_nr = -1;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            robot_nr++;
            robotp = (Robot *)gl->data;

            points[robot_nr] = 0;
            for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
              {
                stat_t * statp = (stat_t*)(stat_gl->data);
                if(statp->sequence_nr == stat_looking_at_nr)
                  points[robot_nr] += statp->points;
              }
          }

        for(int i = 0;i<number_of_robots;i++)
          {
            int temp_pos = 1;
            for(int j = 0;j<number_of_robots;j++)
              if(points[j] > points[i])
                temp_pos++;
            position[i] = temp_pos;
          }

        robot_nr = -1;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            robot_nr++;
            robotp = (Robot *)gl->data;
            stat_t average_stat(0,0,0,0.0,0.0,0.0);
            stat_t * statp = NULL;
            int number_of_stat_found = 0;

            for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
              {
                statp = (stat_t*)(stat_gl->data);
                if(statp->sequence_nr == stat_looking_at_nr)
                  {
                    number_of_stat_found++;
                    average_stat.points += statp->points;
                    average_stat.time_survived += statp->time_survived;
                    average_stat.total_points += statp->points;
                  }
              }
            if( number_of_stat_found > 0 )
              {
                average_stat.position = position[robot_nr];
                average_stat.points /= number_of_stat_found;
                average_stat.time_survived /= number_of_stat_found;
                add_new_row( robotp, &average_stat, number_of_stat_found );
              }
          }
        break;
      }
    case STAT_TABLE_GAME:
      {
        int looking_at_game = stat_looking_at_nr % the_arena.get_games_per_sequence();
        int looking_at_sequence = (stat_looking_at_nr / the_arena.get_games_per_sequence()) + 1;
        if( looking_at_game == 0 )
          {
            looking_at_game = the_arena.get_games_per_sequence();
            looking_at_sequence--;
          }

        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            robotp = (Robot *)gl->data;
            stat_t * statp = NULL;

            for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
              {
                statp = (stat_t*)(stat_gl->data);
                if(statp->sequence_nr == looking_at_sequence && statp->game_nr == looking_at_game)
                  add_new_row( robotp, statp, -1 );
              }
          }
        break;
      }
    case STAT_TABLE_ROBOT:
      {
        int i=0;
        stat_t * statp = NULL;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            i++;
            robotp = (Robot *)gl->data;
            if(i==stat_looking_at_nr)
              for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
                {
                  statp = (stat_t*)(stat_gl->data);
                  add_new_row( robotp, statp, -1 );
                }
          }
        break;
      }
    }  
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_clist_sort(GTK_CLIST(stat_clist));
#endif
  gtk_clist_thaw(GTK_CLIST(stat_clist));
}

void
Gui::stat_make_title_button()
{
  if( stat_title_hbox != NULL )
    gtk_widget_destroy( stat_title_hbox );
  stat_title_hbox = gtk_hbox_new (FALSE, 5);
  gtk_container_add( GTK_CONTAINER(stat_title_button), stat_title_hbox );
  gtk_widget_show (stat_title_hbox);

  switch(stat_table_type)
    {
    case STAT_TABLE_TOTAL:
      {
        GtkWidget * label = gtk_label_new("Grand Total");
        gtk_box_pack_start (GTK_BOX (stat_title_hbox), label, TRUE, TRUE, 0);
        gtk_widget_show( label );
      }
      break;
    case STAT_TABLE_SEQUENCE:
      {
        GtkWidget * label = gtk_label_new(String("Sequence: " + String(stat_looking_at_nr)).chars());
        gtk_box_pack_start (GTK_BOX (stat_title_hbox), label, TRUE, TRUE, 0);
        gtk_widget_show( label );
      }
      break;
    case STAT_TABLE_GAME:
      {
        int looking_at_game = stat_looking_at_nr % the_arena.get_games_per_sequence();
        int looking_at_sequence = (stat_looking_at_nr / the_arena.get_games_per_sequence()) + 1;
        if( looking_at_game == 0 )
          {
            looking_at_game = the_arena.get_games_per_sequence();
            looking_at_sequence--;
          }

        GtkWidget * label = gtk_label_new(String("Sequence: " + String(looking_at_sequence) + "  Game: " + String(looking_at_game)).chars());
        gtk_box_pack_start (GTK_BOX (stat_title_hbox), label, TRUE, TRUE, 0);
        gtk_widget_show( label );
        break;
      }
    case STAT_TABLE_ROBOT:
      {
        GList* gl;
        Robot* robotp;

        int i=0;
        for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            i++;
            robotp = (Robot *)gl->data;
            if( stat_looking_at_nr == i )
              {
                GdkPixmap * colour_pixmap;
                GdkBitmap * bitmap_mask;

                robotp->get_stat_pixmap(statistics_window->window, colour_pixmap, bitmap_mask);

                GtkWidget * pixmap_widget = gtk_pixmap_new( colour_pixmap, bitmap_mask );
                gtk_box_pack_start (GTK_BOX (stat_title_hbox), pixmap_widget, FALSE, FALSE, 0);
                gtk_widget_show( pixmap_widget );

                GtkWidget * label = gtk_label_new(robotp->get_robot_name().non_const_chars());

                gtk_box_pack_start (GTK_BOX (stat_title_hbox), label, TRUE, FALSE, 0);
                gtk_widget_show( label );
              }
          }
        break;
      }
    }
}

void
Gui::setup_statistics_window()
{
  GtkWidget * vbox, * hbox;
  GtkWidget * button_widget;

  stat_table_type = STAT_TABLE_TOTAL;
  stat_looking_at_nr = ( the_arena.get_sequence_nr() - 1 ) * the_arena.get_games_per_sequence()
    + the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence();

  statistics_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (statistics_window), "RealTimeBattle Statistics");
  gtk_widget_set_name (statistics_window, "RTB Statistics");
  gtk_signal_connect (GTK_OBJECT (statistics_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(statistics_window));
  gtk_container_border_width (GTK_CONTAINER (statistics_window), 12);  
  gtk_widget_set_usize(statistics_window,
                       (int)the_opts.get_l(OPTION_STATISTICS_WINDOW_SIZE_X),(int)the_opts.get_l(OPTION_STATISTICS_WINDOW_SIZE_Y));

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (statistics_window), vbox);
  gtk_widget_show (vbox);

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  // Buttons for displaying different statistics

  button_widget = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button_widget), "clicked",
                      GTK_SIGNAL_FUNC (statistics_button_callback), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button_widget, TRUE, TRUE, 0);
  gtk_widget_show (button_widget);

  button_widget = gtk_button_new_with_label ("Save Statistics");
  gtk_signal_connect (GTK_OBJECT (button_widget), "clicked",
                      GTK_SIGNAL_FUNC (save_statistics_callback), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button_widget, TRUE, TRUE, 0);
  gtk_widget_show (button_widget);

  // Upper line of buttons

  char * upper_button_labels[] = { "Total", "Sequence Total", "Game", "Robot" };
  for(int button=STAT_BUTTON_TOTAL;button<=STAT_BUTTON_ROBOT;button++)
    {
      button_widget = gtk_button_new_with_label (upper_button_labels[button]);
      int * type_p;
      type_p = new int(button);
      gtk_signal_connect (GTK_OBJECT (button_widget), "clicked",
                          GTK_SIGNAL_FUNC (buttons_in_statistics_callback), (gpointer) type_p);
      gtk_box_pack_start (GTK_BOX (hbox), button_widget, TRUE, TRUE, 0);
      gtk_widget_show (button_widget);
    }

  // Lower line of buttons

  const char * lower_button_xpms[4][13] = {
    {"13 10 2 1",
     "       c None",
     "x      c #000000000000",
     "xx         xx",
     "xx       xxxx",  
     "xx     xxxxxx",
     "xx   xxxxxxxx",
     "xx  xxxxxxxxx",
     "xx  xxxxxxxxx",
     "xx   xxxxxxxx",
     "xx     xxxxxx",
     "xx       xxxx",
     "xx         xx"},
    {"9 10 2 1",
     "       c None",
     "x      c #000000000000",
     "       xx",
     "     xxxx",  
     "   xxxxxx",
     " xxxxxxxx",
     "xxxxxxxxx",
     "xxxxxxxxx",
     " xxxxxxxx",
     "   xxxxxx",
     "     xxxx",
     "       xx"},
    {"9 10 2 1",
     "       c None",
     "x      c #000000000000",
     "xx       ",
     "xxxx     ",  
     "xxxxxx   ",
     "xxxxxxxx ",
     "xxxxxxxxx",
     "xxxxxxxxx",
     "xxxxxxxx ",
     "xxxxxx   ",
     "xxxx     ",
     "xx       "},
    {"13 10 2 1",
     "       c None",
     "x      c #000000000000",
     "xx         xx",
     "xxxx       xx",  
     "xxxxxx     xx",
     "xxxxxxxx   xx",
     "xxxxxxxxx  xx",
     "xxxxxxxxx  xx",
     "xxxxxxxx   xx",
     "xxxxxx     xx",
     "xxxx       xx",
     "xx         xx"}};

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  gtk_widget_show(statistics_window);

  int made_middle_button = 0;
  for(int button=STAT_BUTTON_FIRST;button<=STAT_BUTTON_LAST;button++)
    {
      if( button != STAT_BUTTON_UPDATE)
        {
          GdkPixmap * pixmap;
          GdkBitmap * bitmap_mask;
          pixmap = gdk_pixmap_create_from_xpm_d( statistics_window->window, &bitmap_mask,
                                                 the_arena.get_background_colour_p(),
                                                 (gchar **)lower_button_xpms[button-STAT_BUTTON_FIRST-made_middle_button] );
          GtkWidget * pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
          gtk_widget_show( pixmap_widget );
          button_widget = gtk_button_new();
          gtk_container_add( GTK_CONTAINER(button_widget), pixmap_widget );

          int * type_p;
          type_p = new int(button);
          gtk_signal_connect (GTK_OBJECT (button_widget), "clicked",
                              GTK_SIGNAL_FUNC (buttons_in_statistics_callback), (gpointer) type_p);
          gtk_widget_set_usize(button_widget, 28,14);
          gtk_box_pack_start (GTK_BOX (hbox), button_widget, FALSE, FALSE, 0);
          gtk_widget_show (button_widget);
        }
      else if( button == STAT_BUTTON_UPDATE )
        {
          stat_title_hbox = NULL;
          stat_title_button = gtk_button_new();
          int * type_p;
          type_p = new int(button);
          gtk_signal_connect (GTK_OBJECT (stat_title_button), "clicked",
                              GTK_SIGNAL_FUNC (buttons_in_statistics_callback), (gpointer) type_p);
          gtk_box_pack_start (GTK_BOX (hbox), stat_title_button, TRUE, TRUE, 0);
          gtk_widget_show (stat_title_button);

          made_middle_button = 1;
        }
    }

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  GtkObject* hadj = gtk_adjustment_new ( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkObject* vadj = gtk_adjustment_new ( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkWidget* scrolled_win = gtk_scrolled_window_new (GTK_ADJUSTMENT ( hadj ),
                                                     GTK_ADJUSTMENT ( vadj ) );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_win ),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
  gtk_widget_show ( scrolled_win );
#endif

  char * titles[7] = { "","  Name", "Position", "Points", "Games ", "Survival Time", "Total Points" };
  stat_clist = gtk_clist_new_with_titles(7, titles);
  gtk_clist_set_selection_mode (GTK_CLIST(stat_clist), GTK_SELECTION_BROWSE);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 0, 5);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 1, 120);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 2, 45);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 3, 35);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 4, 45);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 5, 75);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 6, 60);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_CENTER);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 2, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 3, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 4, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 5, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 6, GTK_JUSTIFY_RIGHT);
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_clist_set_shadow_type(GTK_CLIST(stat_clist), GTK_SHADOW_IN);
  gtk_clist_set_compare_func(GTK_CLIST(stat_clist),float_compare);
  gtk_clist_set_sort_column(GTK_CLIST(stat_clist),6);
  gtk_clist_set_sort_type(GTK_CLIST(stat_clist),GTK_SORT_DESCENDING);
  gtk_signal_connect (GTK_OBJECT (stat_clist), "click_column",
                      (GtkSignalFunc) stat_clist_column_callback, NULL);
  gtk_container_add(GTK_CONTAINER(scrolled_win), stat_clist);
#else
  gtk_clist_set_border(GTK_CLIST(stat_clist), GTK_SHADOW_IN);
  gtk_clist_set_policy(GTK_CLIST(stat_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX (vbox), stat_clist, TRUE, TRUE, 0);
#endif
  gtk_widget_show(stat_clist);

  add_the_statistics_to_clist();
  statistics_up = true;
}

void
Gui::close_statistics_window()
{
  statistics_up = false;
  gtk_widget_destroy ( statistics_window );
}
