#include <strstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>
#include "gui.h"

void
statistics_button_callback(GtkWidget *widget, gpointer guip)
{
  if(((Gui *)guip)->get_statistics_up() == false)
    ((Gui *)guip)->setup_statistics_window();
  else
    ((Gui *)guip)->close_statistics_window();
}

void
buttons_in_statistics_callback(GtkWidget *widget, gpointer bi_p)
{
  button_info_t * button_info_p = (button_info_t *)bi_p;
  switch(button_info_p->type)
    {
    case STAT_BUTTON_TOTAL:
      break;
    case STAT_BUTTON_SEQUENCE:
      break;
    case STAT_BUTTON_GAME:
      break;
    case STAT_BUTTON_ROBOT:
      break;
    case STAT_BUTTON_FIRST:
      break;
    case STAT_BUTTON_PREV:
      button_info_p->guip->change_statistics( -1 );
      break;
    case STAT_BUTTON_NEXT:
      button_info_p->guip->change_statistics( 1 );
      break;
    case STAT_BUTTON_LAST:
      break;
    }
}

void
Gui::change_statistics( int change )
{
  int old_look = stat_looking_at_game;
  stat_looking_at_game += change;
  int game_nr = the_arena->get_games_per_sequence() - the_arena->get_games_remaining_in_sequence();
  if(stat_looking_at_game <= 0 )
    {
      if(stat_looking_at_sequence >= 2)
        {
          stat_looking_at_sequence--;
          stat_looking_at_game = the_arena->get_games_per_sequence();
        }
      else
        stat_looking_at_game = old_look;
    }
  else if((stat_looking_at_game > game_nr && stat_looking_at_sequence == the_arena->get_sequence_nr()) ||
          (stat_looking_at_game > the_arena->get_games_per_sequence() && stat_looking_at_sequence < the_arena->get_sequence_nr()))
    {
      if(stat_looking_at_sequence < the_arena->get_sequence_nr())
        {
          stat_looking_at_sequence++;
          stat_looking_at_game = 1;
        }
      else
        stat_looking_at_game = old_look;
    }

  if( stat_looking_at_game != old_look)
    add_the_statistics_to_clist();
}

void
Gui::add_the_statistics_to_clist()
{
  GList* gl, * stat_gl;
  Robot* robotp;

  gtk_clist_clear(GTK_CLIST(stat_clist));
  for(gl = g_list_next(the_arena->get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot *)gl->data;
      stat_t * statp = NULL;

      for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
        {
          statp = (stat_t*)(stat_gl->data);
          if(statp->sequence_nr == stat_looking_at_sequence && statp->game_nr == stat_looking_at_game)
            {
              strstream ss0,ss1,ss2,ss3,ss4;

              char * list[5];

              ss0 << robotp->get_robotname();
              list[0] = new char[30];
              ss0 >> list[0];

              ss1 << statp->position;
              list[1] = new char[30];
              ss1 >> list[1];

              ss2 << statp->points;
              list[2] = new char[30];
              ss2 >> list[2];

              ss3 << setiosflags(ios::fixed) << setprecision(2) << statp->time_survived;
              list[3] = new char[30];
              ss3 >> list[3];

              ss4 << statp->total_points;
              list[4] = new char[30];
              ss4 >> list[4];

              int row = gtk_clist_append(GTK_CLIST(stat_clist), list);
              gtk_clist_set_foreground(GTK_CLIST(stat_clist), row, the_arena->get_foreground_colour_p());
              gtk_clist_set_background(GTK_CLIST(stat_clist), row, the_arena->get_background_colour_p());
            }
        }
    }  
}

void
Gui::setup_statistics_window()
{
  GtkWidget * vbox;
  GtkWidget * button_table;
  GtkWidget * button;

  statistics_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_window_set_title (GTK_WINDOW (statistics_window), "RealTimeBattle Statistics");
  gtk_signal_connect (GTK_OBJECT (statistics_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(statistics_window));
  gtk_container_border_width (GTK_CONTAINER (statistics_window), 12);  

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (statistics_window), vbox);
  gtk_widget_show (vbox);

  button_table = gtk_table_new (2, 5, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox), button_table, FALSE, FALSE, 0);

  // Buttons for displaying different statistics

  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (statistics_button_callback), (gpointer) this);
  gtk_table_attach_defaults (GTK_TABLE(button_table), button, 0, 1, 0, 1);
  gtk_widget_show (button);

  // Upper line of buttons

  char * upper_button_labels[] = { "Total", "Sequence Total", "Game", "Robot" };
  for(int i=1;i<5;i++)
    {
      button = gtk_button_new_with_label (upper_button_labels[i-1]);
      button_info_t * data;
      data = new button_info_t( this, (stat_button_t)(i - 1) );
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (buttons_in_statistics_callback), (gpointer) data);
      gtk_table_attach_defaults (GTK_TABLE(button_table), button, i, i + 1, 0, 1);
      gtk_widget_show (button);
    }

  // Upper line of buttons

  char * lower_button_labels[] = { "First", "Prev", "Next", "Last" };
  for(int i=1;i<5;i++)
    {
      button = gtk_button_new_with_label (lower_button_labels[i-1]);
      button_info_t * data;
      data = new button_info_t( this, (stat_button_t)(i + 3) );
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (buttons_in_statistics_callback), (gpointer) data);
      gtk_table_attach_defaults (GTK_TABLE(button_table), button, i, i + 1, 1, 2);
      gtk_widget_show (button);
    }

  gtk_table_set_row_spacings (GTK_TABLE(button_table), 5);
  gtk_table_set_col_spacings (GTK_TABLE(button_table), 5);
  gtk_widget_show (button_table);

  char * titles[5] = { "Name", "Position", "Points", "Survival Time", "Total Points" };

  stat_clist = gtk_clist_new_with_titles( 5, titles);
  gtk_clist_set_selection_mode (GTK_CLIST(stat_clist), GTK_SELECTION_BROWSE);
  gtk_clist_set_border(GTK_CLIST(stat_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 0, 80);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 1, 45);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 2, 35);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 3, 75);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 4, 65);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 2, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 3, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 4, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_policy(GTK_CLIST(stat_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(stat_clist, 340,350);
  gtk_box_pack_start (GTK_BOX (vbox), stat_clist, TRUE, TRUE, 0);
  gtk_widget_show(stat_clist);

  stat_table_type = STAT_TABLE_GAME;
  stat_looking_at_sequence = the_arena->get_sequence_nr();
  stat_looking_at_game = the_arena->get_games_per_sequence() - the_arena->get_games_remaining_in_sequence();

  gtk_widget_show(statistics_window);

  add_the_statistics_to_clist();
  statistics_up = true;
}

void
Gui::close_statistics_window()
{
  statistics_up = false;
  gtk_widget_destroy ( statistics_window );
}
