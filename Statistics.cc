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
      button_info_p->guip->change_stat_type( STAT_TABLE_TOTAL );
      break;
    case STAT_BUTTON_SEQUENCE:
      button_info_p->guip->change_stat_type( STAT_TABLE_SEQUENCE );
      break;
    case STAT_BUTTON_GAME:
      button_info_p->guip->change_stat_type( STAT_TABLE_GAME );
      break;
    case STAT_BUTTON_ROBOT:
      button_info_p->guip->change_stat_type( STAT_TABLE_ROBOT );
      break;
    case STAT_BUTTON_FIRST:
      button_info_p->guip->change_statistics( -1, true );
      break;
    case STAT_BUTTON_PREV:
      button_info_p->guip->change_statistics( -1, false );
      break;
    case STAT_BUTTON_NEXT:
      button_info_p->guip->change_statistics( 1, false );
      break;
    case STAT_BUTTON_LAST:
      button_info_p->guip->change_statistics( 1, true );
      break;
    }
}

void
Gui::change_stat_type( stat_table_t type )
{
  gtk_clist_freeze(GTK_CLIST(stat_clist));
  if(stat_table_type == STAT_TABLE_ROBOT && type != STAT_TABLE_ROBOT)
    {
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 0, "");
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 1, "Name");
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 0, 20);
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 1, 80);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_CENTER);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_LEFT);
    }
  if(stat_table_type != STAT_TABLE_ROBOT && type == STAT_TABLE_ROBOT)
    {
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 0, "Seq");
      gtk_clist_set_column_title(GTK_CLIST(stat_clist), 1, "Game");
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 0, 25);
      gtk_clist_set_column_width(GTK_CLIST(stat_clist), 1, 30);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_RIGHT);
      gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_RIGHT);
    }
  stat_table_type = type;
  switch( type )
    {
    case STAT_TABLE_TOTAL:
      break;
    case STAT_TABLE_SEQUENCE:
      stat_looking_at_nr = the_arena->get_sequence_nr();
      break;
    case STAT_TABLE_GAME:
      stat_looking_at_nr = ( the_arena->get_sequence_nr() - 1 ) * the_arena->get_games_per_sequence()
        + the_arena->get_games_per_sequence() - the_arena->get_games_remaining_in_sequence();
      break;
    case STAT_TABLE_ROBOT:
      stat_looking_at_nr = 1;
      break;
    }
  add_the_statistics_to_clist();
}

void
Gui::change_statistics( int change, bool absolut_change )
{
  int old_look = stat_looking_at_nr;
  int game_nr = the_arena->get_games_per_sequence() - the_arena->get_games_remaining_in_sequence();

  int max_nr = 0;
  switch(stat_table_type)
    {
    case STAT_TABLE_TOTAL:
      max_nr = -1;
      break;
    case STAT_TABLE_SEQUENCE:
      max_nr = the_arena->get_sequence_nr();
      break;
    case STAT_TABLE_GAME:
      max_nr = ( the_arena->get_sequence_nr() - 1 ) * the_arena->get_games_per_sequence() + game_nr;
      break;
    case STAT_TABLE_ROBOT:
      GList* gl;
      for(gl = g_list_next(the_arena->get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
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

  if( stat_looking_at_nr != old_look)
    add_the_statistics_to_clist();
}

void
Gui::add_new_row( void * rp, void * sp )
{
  strstream ss0,ss2,ss3,ss4,ss5,ss6,ss7;

  Robot * robotp = (Robot *)rp;
  stat_t * statp = (stat_t *)sp;
  char * list[8];
              
  for(int j=0;j<8;j++)
    {
      list[j] = new char[30];
      strcpy(list[j],"");
    }

  int row = gtk_clist_append(GTK_CLIST(stat_clist), list);
  gtk_clist_set_foreground(GTK_CLIST(stat_clist), row, the_arena->get_foreground_colour_p());
  gtk_clist_set_background(GTK_CLIST(stat_clist), row, the_arena->get_background_colour_p());
  char str[30],str2[25];

  if(stat_table_type == STAT_TABLE_GAME)
    {
      char * colour_square[] =
      {
        "14 14 1 1",
        ".      c #000000000000   ",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        "..............",
        ".............."
      };

      GdkPixmap * colour_pixmap;
      GdkBitmap * bitmap_mask;

      ss0 << ".      c #" << setw(4) << setfill('0') << hex << robotp->get_colour().red
          << robotp->get_colour().green << robotp->get_colour().blue << ends << endl;
      ss0.getline(str2,25,'\n');
      char* newstr = new char[30];
      strcpy(newstr, str2);
      colour_square[1] = newstr;

      colour_pixmap = gdk_pixmap_create_from_xpm_d( statistics_window->window, &bitmap_mask,
                                                    the_arena->get_background_colour_p(),
                                                    (gchar **)colour_square );

      gtk_clist_set_pixmap(GTK_CLIST(stat_clist), row, 0, colour_pixmap, bitmap_mask);
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 1, robotp->get_robotname());
    }

  if(stat_table_type == STAT_TABLE_ROBOT)
    {
      ss2 << statp->sequence_nr;
      ss2 >> str;
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 0, str);

      ss3 << statp->game_nr;
      ss3 >> str;
      gtk_clist_set_text(GTK_CLIST(stat_clist), row, 1, str);
    }

  ss4 << statp->position;
  ss4 >> str;
  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 2, str);

  ss5 << statp->points;
  ss5 >> str;
  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 3, str);

  ss6 << setiosflags(ios::fixed) << setprecision(2) << statp->time_survived;
  ss6 >> str;
  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 4, str);

  ss7 << statp->total_points;
  ss7 >> str;
  gtk_clist_set_text(GTK_CLIST(stat_clist), row, 5, str);
}

void
Gui::add_the_statistics_to_clist()
{
  GList* gl, * stat_gl;
  Robot* robotp;

  gtk_clist_freeze(GTK_CLIST(stat_clist));
  gtk_clist_clear(GTK_CLIST(stat_clist));
  switch( stat_table_type )
    {
    case STAT_TABLE_TOTAL:
      break;
    case STAT_TABLE_SEQUENCE:
      break;
    case STAT_TABLE_GAME:
      {
        int looking_at_game = stat_looking_at_nr % the_arena->get_games_per_sequence();
        int looking_at_sequence = (stat_looking_at_nr / the_arena->get_games_per_sequence()) + 1;
        if( looking_at_game == 0 )
          {
            looking_at_game = the_arena->get_games_per_sequence();
            looking_at_sequence--;
          }

        for(gl = g_list_next(the_arena->get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            robotp = (Robot *)gl->data;
            stat_t * statp = NULL;

            for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
              {
                statp = (stat_t*)(stat_gl->data);
                if(statp->sequence_nr == looking_at_sequence && statp->game_nr == looking_at_game)
                  add_new_row( robotp, statp );
              }
          }
        break;
      }
    case STAT_TABLE_ROBOT:
      {
        int i=0;
        stat_t * statp = NULL;
        for(gl = g_list_next(the_arena->get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
          {
            i++;
            robotp = (Robot *)gl->data;
            if(i==stat_looking_at_nr)
              for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
                {
                  statp = (stat_t*)(stat_gl->data);
                  add_new_row( robotp, statp );
                }
          }
        break;
      }
    }  
  gtk_clist_thaw(GTK_CLIST(stat_clist));
}

void
Gui::setup_statistics_window()
{
  GtkWidget * vbox, * hbox;
  GtkWidget * button;

  statistics_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_window_set_title (GTK_WINDOW (statistics_window), "RealTimeBattle Statistics");
  gtk_signal_connect (GTK_OBJECT (statistics_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(statistics_window));
  gtk_container_border_width (GTK_CONTAINER (statistics_window), 12);  

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (statistics_window), vbox);
  gtk_widget_show (vbox);

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show (hbox);

  // Buttons for displaying different statistics

  button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (statistics_button_callback), (gpointer) this);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
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
      gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
      gtk_widget_show (button);
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
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show (hbox);

  for(int i=1;i<5;i++)
    {
      GdkPixmap * pixmap;
      GdkBitmap * bitmap_mask;
      pixmap = gdk_pixmap_create_from_xpm_d( score_window->window, &bitmap_mask,
                                             the_arena->get_background_colour_p(),
                                             (gchar **)lower_button_xpms[i-1] );
      GtkWidget * pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
      gtk_widget_show( pixmap_widget );
      button = gtk_button_new();
      gtk_container_add( GTK_CONTAINER(button), pixmap_widget );

      button_info_t * data;
      data = new button_info_t( this, (stat_button_t)(i + 3) );
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (buttons_in_statistics_callback), (gpointer) data);
      gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
      gtk_widget_show (button);
    }

  char * titles[6] = { "","Name", "Position", "Points", "Survival Time", "Total Points" };
  stat_clist = gtk_clist_new_with_titles(6, titles);
  gtk_clist_set_selection_mode (GTK_CLIST(stat_clist), GTK_SELECTION_BROWSE);
  gtk_clist_set_border(GTK_CLIST(stat_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 0, 20);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 1, 80);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 2, 45);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 3, 35);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 4, 75);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 5, 65);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_CENTER);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 2, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 3, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 4, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 5, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_policy(GTK_CLIST(stat_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(stat_clist, 380,350);
  gtk_box_pack_start (GTK_BOX (vbox), stat_clist, TRUE, TRUE, 0);
  gtk_widget_show(stat_clist);

  stat_table_type = STAT_TABLE_GAME;
  stat_looking_at_nr = ( the_arena->get_sequence_nr() - 1 ) * the_arena->get_games_per_sequence()
    + the_arena->get_games_per_sequence() - the_arena->get_games_remaining_in_sequence();

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
