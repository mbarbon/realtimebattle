#include <fstream.h>
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

  all_double_options[OPTION_CHECK_INTERVAL] = 
    option_info_t<double>(ENTRY_DOUBLE, 1.0, 0.01, 1000000, 12, "Process check interval", NULL);
    
  all_long_options[OPTION_ROBOT_ENERGY_LEVELS] = 
    option_info_t<long>(ENTRY_INT, 10, 1, 100, 4, "Robot energy levels", NULL);

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

  all_double_options[OPTION_ARENA_SCALE] = 
    option_info_t<double>(ENTRY_DOUBLE, 1.0, 0.001, 1000, 12, "Arena scale", NULL);

  all_double_options[OPTION_MAX_TIMESTEP] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.1, 0.001, 1.0, 12, "Max timestep", NULL);

  all_double_options[OPTION_TIMESCALE] = 
    option_info_t<double>(ENTRY_DOUBLE, 1.0, 0.01, 100.0, 12, "Timescale", NULL);

  all_double_options[OPTION_TIMEOUT] = 
    option_info_t<double>(ENTRY_DOUBLE, 120.0, 1.0, 100000000.0, 12, "Timeout [s]", NULL);

  all_double_options[OPTION_CPU_START_LIMIT] = 
    option_info_t<double>(ENTRY_DOUBLE, 5.0, 0.01, 100000000.0, 12, "Start CPU time [s]", NULL);

  all_double_options[OPTION_CPU_EXTRA] = 
    option_info_t<double>(ENTRY_DOUBLE, 2.5, 0.01, 100000000.0, 12, "Extra CPU time [s]", NULL);

  all_double_options[OPTION_CPU_PERIOD] = 
    option_info_t<double>(ENTRY_DOUBLE, 60.0, 1.0, 100000000.0, 12, "Minimum time to wait for more CPU time [s]", NULL);

  all_double_options[OPTION_CPU_WARNING_PERCENT] = 
    option_info_t<double>(ENTRY_DOUBLE, 0.9, 0.1, 1.0, 12, "CPU time warning percentage", NULL);

  all_string_options[OPTION_STATISTICS_SAVE_FILE] =
    option_info_t<String>(ENTRY_CHAR, "statistics.txt", "", "", 100, "File to save the statistics", NULL);

  all_string_options[OPTION_OPTIONS_SAVE_FILE] =
    option_info_t<String>(ENTRY_CHAR, "options.txt", "", "", 100, "File to save the options", NULL);

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
  the_arena.broadcast( GAME_OPTION, ROBOT_ENERGY_LEVELS, get_l(OPTION_ROBOT_ENERGY_LEVELS));

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
Options::set_all_options_from_gui()
{
  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    {
      double entry_value = str2dbl(gtk_entry_get_text(GTK_ENTRY(all_double_options[i].entry)));
      if( entry_value > all_double_options[i].max_value )
        entry_value = all_double_options[i].max_value;
      if( entry_value < all_double_options[i].min_value )
        entry_value = all_double_options[i].min_value;

      all_double_options[i].value = entry_value;
    }

  for(int i=0;i<LAST_LONG_OPTION;i++)
    {
      long entry_value = 0;
      if( all_long_options[i].datatype == ENTRY_INT )
        entry_value = str2long(gtk_entry_get_text(GTK_ENTRY(all_long_options[i].entry)));
      if( all_long_options[i].datatype == ENTRY_HEX )
        entry_value = str2hex(gtk_entry_get_text(GTK_ENTRY(all_long_options[i].entry)));

      if( entry_value > all_long_options[i].max_value )
        entry_value = all_long_options[i].max_value;
      if( entry_value < all_long_options[i].min_value )
        entry_value = all_long_options[i].min_value;

      all_long_options[i].value = entry_value;
    }

  for(int i=0;i<LAST_STRING_OPTION;i++)
    {
      String entry_value(gtk_entry_get_text(GTK_ENTRY(all_string_options[i].entry)));

      all_string_options[i].value = entry_value;
    }

  the_arena.set_colours();

  close_options_window();
}

void
Options::save_all_options_to_file()
{
  String filename(gtk_entry_get_text(GTK_ENTRY(all_string_options[OPTION_OPTIONS_SAVE_FILE].entry)));

  int mode = _IO_OUTPUT;
  ofstream file(filename.chars(), mode);

  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    file << all_double_options[i].label << ": " << all_double_options[i].value << endl;

  for(int i=0;i<LAST_LONG_OPTION;i++)
    {
      if(all_long_options[i].datatype == ENTRY_INT)
        file << all_long_options[i].label << ": " << all_long_options[i].value << endl;
      if(all_long_options[i].datatype == ENTRY_HEX)
        file << all_long_options[i].label << ": " << hex2str(all_long_options[i].value) << endl;
    }

  for(int i=0;i<LAST_STRING_OPTION;i++)
    file << all_string_options[i].label << ": " << all_string_options[i].value << endl;
}

void
Options::setup_options_window()
{
  int number_of_columns = 2;
  int number_of_options = LAST_DOUBLE_OPTION + LAST_LONG_OPTION + LAST_STRING_OPTION + LAST_BOOL_OPTION;
  int options_per_column = (number_of_options + 1) / number_of_columns;

  GtkWidget * description_tables[number_of_columns];
  GtkWidget * entry_tables[number_of_columns];
  GtkWidget * button_tables[number_of_columns];

  options_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (options_window), "RealTimeBattle Options");
  gtk_signal_connect (GTK_OBJECT (options_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(options_window));
  gtk_container_border_width (GTK_CONTAINER (options_window), 12);

  GtkWidget * vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (options_window), vbox);
  gtk_widget_show (vbox);

  GtkWidget * hbox = gtk_hbox_new (FALSE, 30);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show (hbox);

  for(int i=0; i < number_of_columns; i++)
    {
      GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
      gtk_container_add (GTK_CONTAINER (hbox), internal_hbox);
      gtk_widget_show (internal_hbox);

      description_tables[i] = gtk_table_new( options_per_column, 1, TRUE );
      entry_tables[i] = gtk_table_new( options_per_column, 1, TRUE );
      button_tables[i] = gtk_table_new( options_per_column, 3, TRUE );
      gtk_box_pack_start (GTK_BOX (internal_hbox), description_tables[i], TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (internal_hbox), entry_tables[i], TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (internal_hbox), button_tables[i], TRUE, TRUE, 0);
    }

  for( int i=0;i<LAST_DOUBLE_OPTION;i++ )
    {
      int column = i / options_per_column;
      int row = i - column * options_per_column;

      GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
      gtk_table_attach_defaults( GTK_TABLE( description_tables[column] ), internal_hbox, 0, 1, row, row + 1 );
      gtk_widget_show (internal_hbox);

      GtkWidget * label = gtk_label_new(all_double_options[i].label.chars());
      gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, TRUE, 0);
      gtk_widget_show(label);

      all_double_options[i].entry = gtk_entry_new_with_max_length(all_double_options[i].max_letters_in_entry);

      bool sign = false;
      if( all_double_options[i].min_value < 0.0 )
        sign = true;

      gtk_entry_set_text( GTK_ENTRY( all_double_options[i].entry ), String(all_double_options[i].value).chars() );

      entry_t * info = new entry_t( ENTRY_DOUBLE, sign );

      gtk_signal_connect(GTK_OBJECT(all_double_options[i].entry), "changed",
                         GTK_SIGNAL_FUNC(entry_handler), info);
      gtk_widget_set_usize(all_double_options[i].entry, 108 ,18);
      gtk_table_attach_defaults( GTK_TABLE( entry_tables[column] ), all_double_options[i].entry, 0, 1, row, row + 1 );
      gtk_widget_show(all_double_options[i].entry);

      GtkWidget * button = gtk_button_new_with_label ("min");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (double_options_min_callback), (gpointer) &all_double_options[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_tables[column] ), button, 0, 1, row, row + 1 );
      gtk_widget_show (button);

      button = gtk_button_new_with_label ("def");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (double_options_def_callback), (gpointer) &all_double_options[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_tables[column] ), button, 1, 2, row, row + 1 );
      gtk_widget_show (button);

      button = gtk_button_new_with_label ("max");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (double_options_max_callback), (gpointer) &all_double_options[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_tables[column] ), button, 2, 3, row, row + 1 );
      gtk_widget_show (button);
    }

  for( int i=0;i<LAST_LONG_OPTION;i++ )
    {
      int column = (i + LAST_DOUBLE_OPTION) / options_per_column;
      int row = (i + LAST_DOUBLE_OPTION) - column * options_per_column;

      GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
      gtk_table_attach_defaults( GTK_TABLE( description_tables[column] ), internal_hbox, 0, 1, row, row + 1 );
      gtk_widget_show (internal_hbox);

      GtkWidget * label = gtk_label_new(all_long_options[i].label.chars());
      gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, TRUE, 0);
      gtk_widget_show(label);

      all_long_options[i].entry = gtk_entry_new_with_max_length(all_long_options[i].max_letters_in_entry);

      bool sign = false;
      if( all_long_options[i].min_value < 0.0 )
        sign = true;

      if( all_long_options[i].datatype == ENTRY_INT )
        gtk_entry_set_text( GTK_ENTRY( all_long_options[i].entry ), String(all_long_options[i].value).chars() );
      else if (all_long_options[i].datatype == ENTRY_HEX)
        gtk_entry_set_text( GTK_ENTRY( all_long_options[i].entry ), hex2str(all_long_options[i].value).chars() );

      entry_t * info = new entry_t( all_long_options[i].datatype, sign );

      gtk_signal_connect(GTK_OBJECT(all_long_options[i].entry), "changed",
                         GTK_SIGNAL_FUNC(entry_handler), info);
      gtk_widget_set_usize(all_long_options[i].entry, 108,18);
      gtk_table_attach_defaults( GTK_TABLE( entry_tables[column] ), all_long_options[i].entry, 0, 1, row, row + 1 );
      gtk_widget_show(all_long_options[i].entry);

      GtkWidget * button = gtk_button_new_with_label ("min");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (long_options_min_callback), (gpointer) &all_long_options[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_tables[column] ), button, 0, 1, row, row + 1 );
      gtk_widget_show (button);

      button = gtk_button_new_with_label ("def");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (long_options_def_callback), (gpointer) &all_long_options[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_tables[column] ), button, 1, 2, row, row + 1 );
      gtk_widget_show (button);

      button = gtk_button_new_with_label ("max");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (long_options_max_callback), (gpointer) &all_long_options[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_tables[column] ), button, 2, 3, row, row + 1 );
      gtk_widget_show (button);
    }

  for( int i=0;i<LAST_STRING_OPTION;i++ )
    {
      int column = (i + LAST_DOUBLE_OPTION + LAST_LONG_OPTION) / options_per_column;
      int row = (i + LAST_DOUBLE_OPTION + LAST_LONG_OPTION) - column * options_per_column;

      GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
      gtk_table_attach_defaults( GTK_TABLE( description_tables[column] ), internal_hbox, 0, 1, row, row + 1 );
      gtk_widget_show (internal_hbox);

      GtkWidget * label = gtk_label_new(all_string_options[i].label.chars());
      gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, TRUE, 0);
      gtk_widget_show(label);

      all_string_options[i].entry = gtk_entry_new_with_max_length(all_string_options[i].max_letters_in_entry);

      gtk_entry_set_text( GTK_ENTRY( all_string_options[i].entry ), String(all_string_options[i].value).chars() );

      entry_t * info = new entry_t( ENTRY_CHAR, false );

      gtk_signal_connect(GTK_OBJECT(all_string_options[i].entry), "changed",
                         GTK_SIGNAL_FUNC(entry_handler), info);
      gtk_widget_set_usize(all_string_options[i].entry, 108 ,18);
      gtk_table_attach_defaults( GTK_TABLE( entry_tables[column] ), all_string_options[i].entry, 0, 1, row, row + 1 );
      gtk_widget_show(all_string_options[i].entry);

      GtkWidget * button = gtk_button_new_with_label ("def");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (string_options_def_callback), (gpointer) &all_string_options[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_tables[column] ), button, 1, 2, row, row + 1 );
      gtk_widget_show (button);
    }

  for(int i=0; i < number_of_columns; i++)
    {
      gtk_table_set_row_spacings( GTK_TABLE( description_tables[i] ) , 5 );
      gtk_table_set_col_spacings( GTK_TABLE( description_tables[i] ) , 5 );
      gtk_table_set_row_spacings( GTK_TABLE( entry_tables[i] ) , 5 );
      gtk_table_set_col_spacings( GTK_TABLE( entry_tables[i] ) , 5 );
      gtk_table_set_row_spacings( GTK_TABLE( button_tables[i] ) , 5 );
      gtk_table_set_col_spacings( GTK_TABLE( button_tables[i] ) , 5 );
      gtk_widget_show( description_tables[i] );
      gtk_widget_show( entry_tables[i] );
      gtk_widget_show( button_tables[i] );
    }

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show (hbox);

  GtkWidget * button = gtk_button_new_with_label ("Save options");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (save_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Apply");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (apply_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Cancel");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (options_window_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

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
apply_options_requested(GtkWidget * widget, gpointer data)
{
  the_opts.set_all_options_from_gui();
}

void
save_options_requested(GtkWidget * widget, gpointer data)
{
  the_opts.save_all_options_to_file();
}

void
options_window_requested(GtkWidget * widget, gpointer data)
{
  if(the_opts.get_options_window_up() == false)
    the_opts.setup_options_window();
  else
    the_opts.close_options_window();
}

void
double_options_min_callback( GtkWidget * widget, option_info_t<double> * option )
{
  gtk_entry_set_text( GTK_ENTRY( option->entry ), String(option->min_value).chars() );
}

void
double_options_max_callback( GtkWidget * widget, option_info_t<double> * option )
{
  gtk_entry_set_text( GTK_ENTRY( option->entry ), String(option->max_value).chars() );
}

void
double_options_def_callback( GtkWidget * widget, option_info_t<double> * option )
{
  gtk_entry_set_text( GTK_ENTRY( option->entry ), String(option->default_value).chars() );
}

void
long_options_min_callback( GtkWidget * widget, option_info_t<long> * option )
{
  if(option->datatype == ENTRY_INT)
    gtk_entry_set_text( GTK_ENTRY( option->entry ), String(option->min_value).chars() );
  else if(option->datatype == ENTRY_HEX)
    gtk_entry_set_text( GTK_ENTRY( option->entry ), hex2str(option->min_value).chars() );
}

void
long_options_max_callback( GtkWidget * widget, option_info_t<long> * option )
{
  if(option->datatype == ENTRY_INT)
    gtk_entry_set_text( GTK_ENTRY( option->entry ), String(option->max_value).chars() );
  else if(option->datatype == ENTRY_HEX)
    gtk_entry_set_text( GTK_ENTRY( option->entry ), hex2str(option->max_value).chars() );
}

void
long_options_def_callback( GtkWidget * widget, option_info_t<long> * option )
{
  if(option->datatype == ENTRY_INT)
    gtk_entry_set_text( GTK_ENTRY( option->entry ), String(option->default_value).chars() );
  else if(option->datatype == ENTRY_HEX)
    gtk_entry_set_text( GTK_ENTRY( option->entry ), hex2str(option->default_value).chars() );
}

void
string_options_def_callback( GtkWidget * widget, option_info_t<String> * option )
{
  gtk_entry_set_text( GTK_ENTRY( option->entry ), option->default_value.chars() );
}
