#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>

#include "gui.h"
#include "Arena.h"
#include "MovingObject.h"

void
statistics_button_callback(GtkWidget *widget, gpointer data)
{
  if(the_arena.get_state() != Arena::NOT_STARTED)
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
  the_gui.save_statistics_to_file();
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
    gtk_clist_set_column_width(GTK_CLIST(stat_clist), 4, 80);
  else
    gtk_clist_set_column_width(GTK_CLIST(stat_clist), 4, 0);
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
      stat_looking_at_nr = the_arena.get_sequence_nr();
      break;
    case STAT_TABLE_GAME:
      stat_looking_at_nr = ( the_arena.get_sequence_nr() - 1 ) * the_arena.get_games_per_sequence()
        + the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence();
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

  if( stat_looking_at_nr != old_look)
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
  gtk_clist_set_foreground(GTK_CLIST(stat_clist), row, the_arena.get_foreground_colour_p());
  gtk_clist_set_background(GTK_CLIST(stat_clist), row, the_arena.get_background_colour_p());

  if(stat_table_type == STAT_TABLE_GAME ||
     stat_table_type == STAT_TABLE_SEQUENCE ||
     stat_table_type == STAT_TABLE_TOTAL)
    {
      char ** col_sq;
      GdkPixmap * colour_pixmap;
      GdkBitmap * bitmap_mask;

      colour_pixmap = gdk_pixmap_create_from_xpm_d( statistics_window->window, &bitmap_mask,
                                                    the_arena.get_background_colour_p(),
                                                    get_colour_square_xpm( col_sq, robotp->get_colour() ) );

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

            points[robot_nr] = robotp->get_total_points();
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
  gtk_clist_thaw(GTK_CLIST(stat_clist));
}

void
Gui::save_statistics_to_file()
{
  String filename(the_opts.get_s(OPTION_STATISTICS_SAVE_FILE));

  int mode = _IO_OUTPUT;
  ofstream file(filename.chars(), mode);

  GList * gl, * stat_gl;
  Robot * robotp;

  for(gl = g_list_next(the_arena.get_all_robots_in_tournament()); gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot *)gl->data;
      file << robotp->get_robot_name() << ": " << endl;
      for(stat_gl = g_list_next(robotp->get_statistics()); stat_gl != NULL; stat_gl = g_list_next(stat_gl))
        {
          stat_t * statp = (stat_t*)(stat_gl->data);
          file << "Seq: " << statp->sequence_nr << "  Game: " << statp->game_nr << "  Pos: " << statp->position
               << "  Points: " << statp->points << "  Time Survived: " << statp->time_survived
               << "  Total Points: " << statp->total_points << endl;
        }
    }
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
                char ** col_sq;

                colour_pixmap = gdk_pixmap_create_from_xpm_d( statistics_window->window, &bitmap_mask,
                                                              the_arena.get_background_colour_p(),
                                                              get_colour_square_xpm( col_sq, robotp->get_colour() ) );

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

  stat_table_type = STAT_TABLE_GAME;
  stat_looking_at_nr = ( the_arena.get_sequence_nr() - 1 ) * the_arena.get_games_per_sequence()
    + the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence();

  statistics_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_window_set_title (GTK_WINDOW (statistics_window), "RealTimeBattle Statistics");
  gtk_signal_connect (GTK_OBJECT (statistics_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(statistics_window));
  gtk_container_border_width (GTK_CONTAINER (statistics_window), 12);  

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

  char * titles[7] = { "","Name", "Position", "Points", "Games Played", "Survival Time", "Total Points" };
  stat_clist = gtk_clist_new_with_titles(7, titles);
  gtk_clist_set_selection_mode (GTK_CLIST(stat_clist), GTK_SELECTION_BROWSE);
  gtk_clist_set_border(GTK_CLIST(stat_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 0, 20);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 1, 80);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 2, 45);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 3, 35);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 4, 0);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 5, 75);
  gtk_clist_set_column_width (GTK_CLIST(stat_clist), 6, 65);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 0, GTK_JUSTIFY_CENTER);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 1, GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 2, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 3, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 4, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 5, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_column_justification(GTK_CLIST(stat_clist), 6, GTK_JUSTIFY_RIGHT);
  gtk_clist_set_policy(GTK_CLIST(stat_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(stat_clist, 475,350);
  gtk_box_pack_start (GTK_BOX (vbox), stat_clist, TRUE, TRUE, 0);
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
