#include <math.h>
#include "Options.h"
#include "Arena.h"

// void
// Options::set_options()
// {
// }

extern class Arena the_arena;
extern class Gui the_gui;

Options::Options()
{
  all_double_options[OPTION_GRAV_CONST] = 
    option_info_t<double>(ENTRY_DOUBLE, 9.82, 0.2, 20.0, 12,"Gravitational Constant", NULL);

  all_double_options[OPTION_AIR_RESISTANCE] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.005, 0.0, 1.0, 12,"Air resistance", NULL);

  all_double_options[OPTION_ROLL_FRICTION] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.002, 0.0, 1.0, 12,"Roll friction", NULL);

  all_double_options[OPTION_SLIDE_FRICTION] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.1, 0.0, 5.0, 12,"Slide/break friction", NULL);

  all_double_options[OPTION_MAX_ACCELERATION] = 
    option_info_t<double>(ENTRY_DOUBLE, 2.0, 0.1, 10.0, 12,"Max acceleration", NULL);

  all_double_options[OPTION_MIN_ACCELERATION] = 
    option_info_t<double>(ENTRY_DOUBLE, -0.5, -10.0, 0.0, 12,"Min acceleration", NULL);
    
  all_double_options[OPTION_ROBOT_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.5, 0.1, 10.0, 12, "Robot radius", NULL);

  all_double_options[OPTION_ROBOT_MASS] = 
    option_info_t<double>(ENTRY_DOUBLE, 1.0, 0.01, 100.0, 12, "Robot mass", NULL);

  all_double_options[OPTION_ROBOT_BOUNCE_COEFF] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.7, 0.0, 1.0, 12, "Robot bounce coefficient", NULL);

  all_double_options[OPTION_ROBOT_HARDNESS] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.5, 0.0, 1.0, 12, "Robot hardness coefficient", NULL);

  all_double_options[OPTION_ROBOT_PROTECTION] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.5, 0.0, 1.0, 12, "Robot protection coefficient", NULL);

  all_double_options[OPTION_ROBOT_START_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 100.0, 0.01, 10000.0, 12, "Robot start energy", NULL);

  all_double_options[OPTION_ROBOT_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 120.0, 0.01, 10000.0, 12, "Robot max energy", NULL);

  all_double_options[OPTION_ROBOT_FRONT_BOUNCE_COEFF] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.7, 0.0, 1.0, 12, "Robot front bounce coefficient", NULL);

  all_double_options[OPTION_ROBOT_FRONT_HARDNESS] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.9, 0.0, 1.0, 12, "Robot front hardness coefficient", NULL);

  all_double_options[OPTION_ROBOT_FRONT_PROTECTION] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.9, 0.0, 1.0, 12, "Robot front protection coefficient", NULL);

  all_double_options[OPTION_ROBOT_COS_FRONTSIZE] = 
    option_info_t<double>(ENTRY_DOUBLE, cos(M_PI/6.0), -1.0, 1.0, 12, "Robot frontsize (cos(front angle))", NULL);

  all_double_options[OPTION_ROBOT_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, 2.0*M_PI / 8.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0, 12, "Robot max rotate speed [rad/s]", NULL);

  all_double_options[OPTION_ROBOT_CANNON_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, 2.0*M_PI / 4.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0, 12, "Robot cannon max rotate speed [rad/s]", NULL);

  all_double_options[OPTION_ROBOT_RADAR_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, 2.0*M_PI / 3.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0, 12, "Robot radar max rotate speed [rad/s]", NULL);
    
  all_double_options[OPTION_SHOT_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.1, 0.01, 1.0, 12, "Shot radius", NULL);

  all_double_options[OPTION_SHOT_SPEED] = 
    option_info_t<double>(ENTRY_DOUBLE, 10.0, 0.1, 100.0, 12, "Shot speed", NULL);

  all_double_options[OPTION_SHOOTING_PENALTY] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.075, 0.0, 1.0, 12, "Shooting penalty (times shot energy)", NULL);

  all_double_options[OPTION_SHOT_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.5, 0.0, 100.0, 12, "Shot min energy", NULL);

  all_double_options[OPTION_SHOT_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 30.0, 0.0, 100000000.0, 12, "Shot max energy", NULL);

  all_double_options[OPTION_SHOT_ENERGY_INCREASE_SPEED] = 
    option_info_t<double>(ENTRY_DOUBLE, 10.0, 0.0, 100000000.0, 12, "Shot energy increase speed [energypoints/s]", NULL);

  all_long_options[OPTION_BACKGROUND_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, 0xfaf0e6, 0x000000, 0xffffff, 6, "Background colour", NULL);

  all_long_options[OPTION_FOREGROUND_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, 0x000000, 0x000000, 0xffffff, 6, "Foreground colour", NULL);

  all_double_options[OPTION_COOKIE_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 15.0, 0.0, 100000000.0, 12, "Cookie max energy", NULL);

  all_double_options[OPTION_COOKIE_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 10.0, 0.0, 100000000.0, 12, "Cookie min energy", NULL);

  all_double_options[OPTION_COOKIE_FREQUENCY] = 
    option_info_t<double>(ENTRY_DOUBLE, 1.0/30.0, 0.0, 100000000.0, 12, "Cookie frequency [cookies per second]", NULL);

  all_double_options[OPTION_COOKIE_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.3, 0.01, 1.0, 12, "Cookie radius", NULL);

  all_long_options[OPTION_COOKIE_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, 0x35d715, 0x000000, 0xffffff, 6, "Cookie colour", NULL);

  all_double_options[OPTION_MINE_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 25.0, 0.0, 100000000.0, 12, "Mine max energy", NULL);

  all_double_options[OPTION_MINE_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, 15.0, 0.0, 100000000.0, 12, "Mine min energy", NULL);

  all_double_options[OPTION_MINE_FREQUENCY] = 
    option_info_t<double>(ENTRY_DOUBLE, 1.0/15.0, 0.0, 100000000.0, 12, "Mine frequency [mines per second]", NULL);

  all_double_options[OPTION_MINE_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.3, 0.01, 1.0, 12, "Mine radius", NULL);

  all_long_options[OPTION_MINE_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, 0xff0000, 0x000000, 0xffffff, 6, "Mine colour", NULL);

  all_double_options[OPTION_MAX_TIMESTEP] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.5, 0.05, 1.0, 12, "Max timestep", NULL);

  all_double_options[OPTION_TIMEOUT] = 
    option_info_t<double>(ENTRY_DOUBLE, 120.0, 1.0, 100000000.0, 12, "Timeout [s]", NULL);

  options_window_up = false;
}

void
Options::broadcast_opts()
{
  the_arena.broadcast( GAME_OPTION, ROBOT_MAX_ROTATE, get_d(OPTION_ROBOT_MAX_ROTATE));
  the_arena.broadcast( GAME_OPTION, ROBOT_CANNON_MAX_ROTATE, get_d(OPTION_ROBOT_CANNON_MAX_ROTATE));
  the_arena.broadcast( GAME_OPTION, ROBOT_RADAR_MAX_ROTATE, get_d(OPTION_ROBOT_RADAR_MAX_ROTATE));

  the_arena.broadcast( GAME_OPTION, ROBOT_START_ENERGY, get_d(OPTION_ROBOT_START_ENERGY));
  the_arena.broadcast( GAME_OPTION, ROBOT_MAX_ENERGY, get_d(OPTION_ROBOT_MAX_ENERGY));

  the_arena.broadcast( GAME_OPTION, SHOT_SPEED, get_d(OPTION_SHOT_SPEED));
  the_arena.broadcast( GAME_OPTION, SHOT_MIN_ENERGY, get_d(OPTION_SHOT_MIN_ENERGY));
  the_arena.broadcast( GAME_OPTION, SHOT_MAX_ENERGY, get_d(OPTION_SHOT_MAX_ENERGY));
  the_arena.broadcast( GAME_OPTION, SHOT_ENERGY_INCREASE_SPEED, get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED));

  the_arena.broadcast( GAME_OPTION, TIMEOUT, get_d(OPTION_TIMEOUT));  
}

void
Options::get_options_from_rtbrc()
{
}

void
Options::setup_options_window()
{
  int options_per_column = 20;
  int number_of_options = LAST_DOUBLE_OPTION + LAST_LONG_OPTION + LAST_STRING_OPTION + LAST_BOOL_OPTION;
  int number_of_columns = (number_of_options / options_per_column) + 1;
  GtkWidget * vboxes[number_of_columns];

  options_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (options_window), "RealTimeBattle Start New Tournament");
  gtk_signal_connect (GTK_OBJECT (options_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(options_window));
  gtk_container_border_width (GTK_CONTAINER (options_window), 12);

  GtkWidget * hbox = gtk_hbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (options_window), hbox);
  gtk_widget_show (hbox);

  for(int i=0; i < number_of_columns; i++)
    {
      vboxes[i] = gtk_vbox_new (FALSE, 5);
      gtk_container_add (GTK_CONTAINER (hbox), vboxes[i]);
      gtk_widget_show (vboxes[i]);
    }

  for( int i=0;i<LAST_DOUBLE_OPTION;i++ )
    {
      GtkWidget * option_hbox = gtk_hbox_new (FALSE, 5);
      gtk_container_add (GTK_CONTAINER (vboxes[i / options_per_column]), option_hbox);
      gtk_widget_show (option_hbox);

      GtkWidget * label = gtk_label_new(all_double_options[i].label.chars());
      gtk_box_pack_start (GTK_BOX (option_hbox), label, TRUE, FALSE, 0);
      gtk_widget_show(label);

      all_double_options[i].entry = gtk_entry_new_with_max_length(all_double_options[i].max_letters_in_entry);

      bool sign = false;
      if( all_double_options[i].min_value < 0.0 )
        sign = true;

      gtk_entry_set_text( GTK_ENTRY( all_double_options[i].entry ), String(all_double_options[i].value).chars() );

      entry_t * info = new entry_t( ENTRY_DOUBLE, sign );

      gtk_signal_connect(GTK_OBJECT(all_double_options[i].entry), "changed",
                         GTK_SIGNAL_FUNC(entry_handler), info);
      gtk_box_pack_start (GTK_BOX (option_hbox), all_double_options[i].entry, FALSE, FALSE, 0);
      gtk_widget_set_usize(all_double_options[i].entry, all_double_options[i].max_letters_in_entry * 9,18);
      gtk_widget_show(all_double_options[i].entry);
    }

  for( int i=0;i<LAST_LONG_OPTION;i++ )
    {
      GtkWidget * option_hbox = gtk_hbox_new (FALSE, 5);
      gtk_container_add (GTK_CONTAINER (vboxes[(i + LAST_DOUBLE_OPTION) / options_per_column]), option_hbox);
      gtk_widget_show (option_hbox);

      GtkWidget * label = gtk_label_new(all_long_options[i].label.chars());
      gtk_box_pack_start (GTK_BOX (option_hbox), label, TRUE, FALSE, 0);
      gtk_widget_show(label);

      all_long_options[i].entry = gtk_entry_new_with_max_length(all_long_options[i].max_letters_in_entry);

      bool sign = false;
      if( all_long_options[i].min_value < 0.0 )
        sign = true;

      if( all_long_options[i].datatype == ENTRY_INT )
        gtk_entry_set_text( GTK_ENTRY( all_long_options[i].entry ), String(all_long_options[i].value).chars() );
      else if (all_long_options[i].datatype == ENTRY_HEX)
        gtk_entry_set_text( GTK_ENTRY( all_long_options[i].entry ), hex2str(all_long_options[i].value).chars() );

      entry_t * info = new entry_t( ENTRY_DOUBLE, sign );

      gtk_signal_connect(GTK_OBJECT(all_long_options[i].entry), "changed",
                         GTK_SIGNAL_FUNC(entry_handler), info);
      gtk_box_pack_start (GTK_BOX (option_hbox), all_long_options[i].entry, FALSE, FALSE, 0);
      gtk_widget_set_usize(all_long_options[i].entry, all_long_options[i].max_letters_in_entry * 9,18);
      gtk_widget_show(all_long_options[i].entry);
    }

  gtk_widget_show(options_window);
  options_window_up = true;
}

void
Options::close_options_window()
{
  gtk_widget_destroy(options_window);
  options_window_up = false;
}

void
options_window_requested(GtkWidget *widget, gpointer data)
{
  if(the_opts.get_options_window_up() == false)
    the_opts.setup_options_window();
  else
    the_opts.close_options_window();
}
