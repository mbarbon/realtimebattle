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

#include <fstream.h>
#include <math.h>

#include "Gui.h"
#include "Options.h"
//#include "Arena.h"

// void
// Options::set_options()
// {
// }

extern class Arena the_arena;
extern class Gui the_gui;

Options::Options()
{
  filesel_widget = NULL;

  all_double_options[OPTION_GRAV_CONST] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 9.82, 0.2, 20.0, 12,"Gravitational Constant", NULL);

  all_double_options[OPTION_AIR_RESISTANCE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 0.005, 0.0, 1.0, 12,"Air resistance", NULL);

  all_double_options[OPTION_ROLL_FRICTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 0.002, 0.0, 1.0, 12,"Roll friction", NULL);

  all_double_options[OPTION_SLIDE_FRICTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 0.1, 0.0, 5.0, 12,"Slide/brake friction", NULL);

  all_double_options[OPTION_ROBOT_MAX_ACCELERATION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0, 0.1, 10.0, 12,"Robot max acceleration", NULL);

  all_double_options[OPTION_ROBOT_MIN_ACCELERATION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, -0.5, -10.0, 0.0, 12,"Robot min acceleration", NULL);
    
  all_double_options[OPTION_ROBOT_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.5, 0.1, 10.0, 12, "Robot radius", NULL);

  all_double_options[OPTION_ROBOT_MASS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 1.0, 0.01, 100.0, 12, "Robot mass", NULL);

  all_double_options[OPTION_ROBOT_BOUNCE_COEFF] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.7, 0.0, 1.0, 12, "Robot bounce coefficient", NULL);

  all_double_options[OPTION_ROBOT_HARDNESS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.5, 0.0, 1.0, 12, "Robot hardness coefficient", NULL);

  all_double_options[OPTION_ROBOT_PROTECTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.5, 0.0, 1.0, 12, "Robot protection coefficient", NULL);

  all_double_options[OPTION_ROBOT_FRONTSIZE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, M_PI/3.0, 0.0, M_PI*2.0, 12, "Robot frontsize [radians]", NULL);

  all_double_options[OPTION_ROBOT_FRONT_BOUNCE_COEFF] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.7, 0.0, 1.0, 12, "Robot front bounce coefficient", NULL);

  all_double_options[OPTION_ROBOT_FRONT_HARDNESS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.9, 0.0, 1.0, 12, "Robot front hardness coefficient", NULL);

  all_double_options[OPTION_ROBOT_FRONT_PROTECTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.9, 0.0, 1.0, 12, "Robot front protection coefficient", NULL);

  all_double_options[OPTION_ROBOT_START_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 100.0, 0.01, 10000.0, 12, "Robot start energy", NULL);

  all_double_options[OPTION_ROBOT_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 120.0, 0.01, 10000.0, 12, "Robot max energy", NULL);

  all_double_options[OPTION_ROBOT_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0*M_PI / 8.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0, 12, "Robot max rotate speed [rad/s]", NULL);

  all_double_options[OPTION_ROBOT_CANNON_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0*M_PI / 4.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0, 12, "Robot cannon max rotate speed [rad/s]", NULL);

  all_double_options[OPTION_ROBOT_RADAR_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0*M_PI / 3.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0, 12, "Robot radar max rotate speed [rad/s]", NULL);

  all_double_options[OPTION_CHECK_INTERVAL] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 1.0, 0.01, 1000000, 12, "Process check interval", NULL);
    
  all_long_options[OPTION_ROBOT_ENERGY_LEVELS] = 
    option_info_t<long>(ENTRY_INT, PAGE_ROBOT, 10, 1, 100, 4, "Robot energy levels", NULL);

  all_double_options[OPTION_SHOT_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 0.1, 0.01, 1.0, 12, "Shot radius", NULL);

  all_double_options[OPTION_SHOT_SPEED] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 10.0, 0.1, 100.0, 12, "Shot speed", NULL);

  all_double_options[OPTION_SHOOTING_PENALTY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 0.075, 0.0, 1.0, 12, "Shooting penalty (times shot energy)", NULL);

  all_double_options[OPTION_SHOT_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 0.5, 0.0, 100.0, 12, "Shot min energy", NULL);

  all_double_options[OPTION_SHOT_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 30.0, 0.0, 100000000.0, 12, "Shot max energy", NULL);

  all_double_options[OPTION_SHOT_ENERGY_INCREASE_SPEED] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 10.0, 0.0, 100000000.0, 12, "Shot energy increase speed [energy/s]", NULL);

  all_long_options[OPTION_BACKGROUND_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_MISC, 0xfaf0e6, 0x000000, 0xffffff, 6, "Background colour", NULL);

  all_long_options[OPTION_FOREGROUND_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_MISC, 0x000000, 0x000000, 0xffffff, 6, "Foreground colour", NULL);

  all_double_options[OPTION_COOKIE_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 15.0, 0.0, 100000000.0, 12, "Cookie max energy", NULL);

  all_double_options[OPTION_COOKIE_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 10.0, 0.0, 100000000.0, 12, "Cookie min energy", NULL);

  all_double_options[OPTION_COOKIE_FREQUENCY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.03, 0.0, 100000000.0, 12, "Cookie frequency [cookies per second]", NULL);

  all_double_options[OPTION_COOKIE_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.3, 0.01, 1.0, 12, "Cookie radius", NULL);

  all_long_options[OPTION_COOKIE_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_EXTRAS, 0x35d715, 0x000000, 0xffffff, 6, "Cookie colour", NULL);

  all_double_options[OPTION_MINE_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 25.0, 0.0, 100000000.0, 12, "Mine max energy", NULL);

  all_double_options[OPTION_MINE_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 15.0, 0.0, 100000000.0, 12, "Mine min energy", NULL);

  all_double_options[OPTION_MINE_FREQUENCY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.06, 0.0, 100000000.0, 12, "Mine frequency [mines per second]", NULL);

  all_double_options[OPTION_MINE_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.3, 0.01, 1.0, 12, "Mine radius", NULL);

  all_long_options[OPTION_MINE_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_EXTRAS, 0xff0000, 0x000000, 0xffffff, 6, "Mine colour", NULL);

  all_double_options[OPTION_ARENA_SCALE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_MISC, 1.0, 0.001, 1000, 12, "Arena scale", NULL);

  all_double_options[OPTION_TIMEOUT] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 120.0, 1.0, 100000000.0, 12, "Timeout [s]", NULL);

  all_double_options[OPTION_MAX_TIMESTEP] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 0.1, 0.001, 1.0, 12, "Max timestep", NULL);

  all_double_options[OPTION_TIMESCALE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 1.0, 0.01, 100.0, 12, "Timescale", NULL);

  all_double_options[OPTION_CPU_START_LIMIT] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 5.0, 0.01, 100000000.0, 12, "Start CPU time [s]", NULL);

  all_double_options[OPTION_CPU_EXTRA] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 2.5, 0.01, 100000000.0, 12, "Extra CPU time [s]", NULL);

  all_double_options[OPTION_CPU_PERIOD] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 60.0, 1.0, 100000000.0, 12, "Extra CPU period [s]", NULL);

  all_double_options[OPTION_CPU_WARNING_PERCENT] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 0.9, 0.1, 1.0, 12, "CPU time warning percentage", NULL);

  all_string_options[OPTION_ROBOT_SEARCH_PATH] =
    option_info_t<String>(ENTRY_CHAR, PAGE_MISC, "", "", "", 1000, "Robot search path", NULL);

  all_string_options[OPTION_ARENA_SEARCH_PATH] =
    option_info_t<String>(ENTRY_CHAR, PAGE_MISC, "", "", "", 1000, "Arena search path", NULL);

  all_long_options[OPTION_ARENA_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 350, 25, 10000, 6, "Initial Arena window width", NULL);

  all_long_options[OPTION_ARENA_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 350, 25, 10000, 6, "Initial Arena window height", NULL);

  all_long_options[OPTION_MESSAGE_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 294, 50, 10000, 6, "Initial Message window width", NULL);

  all_long_options[OPTION_MESSAGE_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 110, 25, 10000, 6, "Initial Message window height", NULL);

  all_long_options[OPTION_SCORE_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 394, 50, 10000, 6, "Initial Score window width", NULL);

  all_long_options[OPTION_SCORE_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 374, 50, 10000, 6, "Initial Score window height", NULL);

  all_long_options[OPTION_STATISTICS_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 499, 50, 10000, 6, "Initial Statistics window width", NULL);

  all_long_options[OPTION_STATISTICS_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 428, 50, 10000, 6, "Initial Statistics window height", NULL);

  options_window_up = false;
}

void
Options::broadcast_opts()
{
  the_arena.broadcast( GAME_OPTION, ROBOT_MAX_ROTATE, get_d(OPTION_ROBOT_MAX_ROTATE));
  the_arena.broadcast( GAME_OPTION, ROBOT_CANNON_MAX_ROTATE, get_d(OPTION_ROBOT_CANNON_MAX_ROTATE));
  the_arena.broadcast( GAME_OPTION, ROBOT_RADAR_MAX_ROTATE, get_d(OPTION_ROBOT_RADAR_MAX_ROTATE));

  the_arena.broadcast( GAME_OPTION, ROBOT_MAX_ACCELERATION, get_d(OPTION_ROBOT_MAX_ACCELERATION));
  the_arena.broadcast( GAME_OPTION, ROBOT_MIN_ACCELERATION, get_d(OPTION_ROBOT_MIN_ACCELERATION));

  the_arena.broadcast( GAME_OPTION, ROBOT_START_ENERGY, get_d(OPTION_ROBOT_START_ENERGY));
  the_arena.broadcast( GAME_OPTION, ROBOT_MAX_ENERGY, get_d(OPTION_ROBOT_MAX_ENERGY));
  the_arena.broadcast( GAME_OPTION, ROBOT_ENERGY_LEVELS, get_l(OPTION_ROBOT_ENERGY_LEVELS));

  the_arena.broadcast( GAME_OPTION, SHOT_SPEED, get_d(OPTION_SHOT_SPEED));
  the_arena.broadcast( GAME_OPTION, SHOT_MIN_ENERGY, get_d(OPTION_SHOT_MIN_ENERGY));
  the_arena.broadcast( GAME_OPTION, SHOT_MAX_ENERGY, get_d(OPTION_SHOT_MAX_ENERGY));
  the_arena.broadcast( GAME_OPTION, SHOT_ENERGY_INCREASE_SPEED, get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED));

  the_arena.broadcast( GAME_OPTION, TIMEOUT, get_d(OPTION_TIMEOUT));  

  the_arena.broadcast( GAME_OPTION, DEBUG_LEVEL, the_arena.get_debug_level());  
}

void
Options::set_all_options_from_gui()
{
  if((the_arena.get_game_mode() == Arena::COMPETITION_MODE &&
      (the_arena.get_state() == NO_STATE ||
       the_arena.get_state() == NOT_STARTED ||
       the_arena.get_state() == FINISHED)) ||
     (the_arena.get_game_mode() != Arena::COMPETITION_MODE))
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
    }
}

void
Options::get_options_from_rtbrc()
{
  char* home_dir;
  if( NULL == ( home_dir = getenv("HOME") ) )
    return;

  String resource_file = String(home_dir) + "/.rtbrc";
  read_options_file(resource_file,true);
}

void
Options::read_options_file(String file_string, const bool as_default)
{
  ifstream file(file_string.chars());
  if( !file )
    return;

  for(;;)
    {
      char temp;
      char buffer[1000];
      bool option_found_flag = false;

      file >> ws;
      file.get(buffer,100,':');
      file.get(temp);
      String option_name(buffer);
      if(option_name == "")
        break;

      for(int i=0;i<LAST_DOUBLE_OPTION;i++)
        if(option_name == all_double_options[i].label )
          {
            double option_value;
            file >> option_value;
            file.get(buffer,100,'\n');
            all_double_options[i].value = option_value;
            if(as_default)
              all_double_options[i].default_value = option_value;
            option_found_flag = true;
          }

      for(int i=0;i<LAST_LONG_OPTION;i++)
        if(option_name == all_long_options[i].label )
          {
            long option_value = 0;
            if( all_long_options[i].datatype == ENTRY_INT )
              file >> option_value;
            if( all_long_options[i].datatype == ENTRY_HEX )
              {
                String temp_string;
                file >> temp_string;
                while( temp_string[0] == ' ' )
                  temp_string = get_segment(temp_string,1,-1);
                option_value = str2hex(temp_string);
              }
            file.get(buffer,100,'\n');
            all_long_options[i].value = option_value;
            if(as_default)
              all_long_options[i].default_value = option_value;
            option_found_flag = true;
          }

      for(int i=0;i<LAST_STRING_OPTION;i++)
        if(option_name == all_string_options[i].label )
          {
            String option_value;
            file >> option_value;
            while( option_value[0] == ' ' )
              option_value = get_segment(option_value,1,-1);
            file.get(buffer,100,'\n');
            all_string_options[i].value = option_value;
            if(as_default)
              all_string_options[i].default_value = option_value;
            option_found_flag = true;
          }
      if(!option_found_flag)
        file.get(buffer,1000,'\n');        
    }
}

void
Options::save_all_options_to_file(String filename, const bool as_default)
{
  if(as_default)
    {
      char* home_dir;
      if( NULL == ( home_dir = getenv("HOME") ) )
        return;

      filename = String(home_dir) + "/.rtbrc";
    }

  int mode = _IO_OUTPUT;
  ofstream file(filename.chars(), mode);

  if( !file )
    {
      cerr << "Couldn't open save file" << endl;
      return;
    }

  set_all_options_from_gui();

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
Options::update_all_gtk_entries()
{
  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    gtk_entry_set_text( GTK_ENTRY( all_double_options[i].entry ), String(all_double_options[i].value).chars() );
  for(int i=0;i<LAST_LONG_OPTION;i++)
    {
      if( all_long_options[i].datatype == ENTRY_INT )
        gtk_entry_set_text( GTK_ENTRY( all_long_options[i].entry ), String(all_long_options[i].value).chars() );
      else if (all_long_options[i].datatype == ENTRY_HEX)
        gtk_entry_set_text( GTK_ENTRY( all_long_options[i].entry ), hex2str(all_long_options[i].value).chars() );
    }
  for(int i=0;i<LAST_STRING_OPTION;i++)
    gtk_entry_set_text( GTK_ENTRY( all_string_options[i].entry ), String(all_string_options[i].value).chars() );
}

void
Options::revert_all_options_to_default()
{
  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    all_double_options[i].value = all_double_options[i].default_value;
  for(int i=0;i<LAST_LONG_OPTION;i++)
    all_long_options[i].value = all_long_options[i].default_value;
  for(int i=0;i<LAST_STRING_OPTION;i++)
    all_string_options[i].value = all_string_options[i].default_value;
  update_all_gtk_entries();
}

void
Options::setup_options_window()
{
  options_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (options_window), "RealTimeBattle Options");
  gtk_widget_set_name (options_window, "RTB Options");
  gtk_signal_connect (GTK_OBJECT (options_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(options_window));
  gtk_container_border_width (GTK_CONTAINER (options_window), 12);

  GtkWidget * vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (options_window), vbox);
  gtk_widget_show (vbox);

  GtkWidget * notebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos( GTK_NOTEBOOK( notebook ), GTK_POS_LEFT );
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);
  gtk_widget_show( notebook );

  char * page_titles[LAST_PAGE] = { "Environment", "Robot", "Shot", "Extras", "Time", "Window sizes", "Misc" };
  for(int i=0; i < LAST_PAGE; i++)
    {
      int number_of_options = 0;
      for( int opt=0;opt<LAST_DOUBLE_OPTION;opt++ )
        if( all_double_options[opt].page == i )
          number_of_options++;
      for( int opt=0;opt<LAST_LONG_OPTION;opt++ )
        if( all_long_options[opt].page == i )
          number_of_options++;
      for( int opt=0;opt<LAST_STRING_OPTION;opt++ )
        if( all_string_options[opt].page == i )
          number_of_options++;

      GtkWidget * page_vbox = gtk_vbox_new (FALSE, 5);
      gtk_container_border_width( GTK_CONTAINER( page_vbox ), 10 );
      gtk_widget_show (page_vbox);

      GtkWidget * page_hbox = gtk_hbox_new (FALSE, 5);
      gtk_box_pack_start (GTK_BOX (page_vbox), page_hbox, FALSE, FALSE, 0);
      gtk_widget_show (page_hbox);

      GtkWidget * description_table = gtk_table_new( number_of_options, 1, TRUE );
      GtkWidget * entry_table = gtk_table_new( number_of_options, 1, TRUE );
      GtkWidget * button_table = gtk_table_new( number_of_options, 3, TRUE );
      gtk_box_pack_start (GTK_BOX (page_hbox), description_table, TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (page_hbox), entry_table, TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (page_hbox), button_table, TRUE, TRUE, 0);

      int row = -1;

      for( int opt=0;opt<LAST_DOUBLE_OPTION;opt++ )
        if( all_double_options[opt].page == i )
          {
            row++;

            GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
            gtk_table_attach_defaults( GTK_TABLE( description_table ), internal_hbox, 0, 1, row, row + 1 );
            gtk_widget_show (internal_hbox);

            GtkWidget * label = gtk_label_new(all_double_options[opt].label.chars());
            gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, TRUE, 0);
            gtk_widget_show(label);

            all_double_options[opt].entry = gtk_entry_new_with_max_length(all_double_options[opt].max_letters_in_entry);

            bool sign = false;
            if( all_double_options[opt].min_value < 0.0 )
              sign = true;

            gtk_entry_set_text( GTK_ENTRY( all_double_options[opt].entry ), String(all_double_options[opt].value).chars() );

            entry_t * info = new entry_t( ENTRY_DOUBLE, sign );

            gtk_signal_connect(GTK_OBJECT(all_double_options[opt].entry), "changed",
                               GTK_SIGNAL_FUNC(entry_handler), info);
            gtk_widget_set_usize(all_double_options[opt].entry, 108 ,22);
            gtk_table_attach_defaults( GTK_TABLE( entry_table ), all_double_options[opt].entry, 0, 1, row, row + 1 );
            gtk_widget_show(all_double_options[opt].entry);

            GtkWidget * button = gtk_button_new_with_label ("min");
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                                GTK_SIGNAL_FUNC (double_options_min_callback), (gpointer) &all_double_options[opt] );
            gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 0, 1, row, row + 1 );
            gtk_widget_show (button);

            button = gtk_button_new_with_label ("def");
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                                GTK_SIGNAL_FUNC (double_options_def_callback), (gpointer) &all_double_options[opt] );
            gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 1, 2, row, row + 1 );
            gtk_widget_show (button);

            button = gtk_button_new_with_label ("max");
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                                GTK_SIGNAL_FUNC (double_options_max_callback), (gpointer) &all_double_options[opt] );
            gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 2, 3, row, row + 1 );
            gtk_widget_show (button);
          }

      for( int opt=0;opt<LAST_LONG_OPTION;opt++ )
        if( all_long_options[opt].page == i )
          {
            row++;

            GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
            gtk_table_attach_defaults( GTK_TABLE( description_table ), internal_hbox, 0, 1, row, row + 1 );
            gtk_widget_show (internal_hbox);

            GtkWidget * label = gtk_label_new(all_long_options[opt].label.chars());
            gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, TRUE, 0);
            gtk_widget_show(label);

            all_long_options[opt].entry = gtk_entry_new_with_max_length(all_long_options[opt].max_letters_in_entry);

            bool sign = false;
            if( all_long_options[opt].min_value < 0.0 )
              sign = true;

            if( all_long_options[opt].datatype == ENTRY_INT )
              gtk_entry_set_text( GTK_ENTRY( all_long_options[opt].entry ), String(all_long_options[opt].value).chars() );
            else if (all_long_options[opt].datatype == ENTRY_HEX)
              gtk_entry_set_text( GTK_ENTRY( all_long_options[opt].entry ), hex2str(all_long_options[opt].value).chars() );

            entry_t * info = new entry_t( all_long_options[opt].datatype, sign );

            gtk_signal_connect(GTK_OBJECT(all_long_options[opt].entry), "changed",
                               GTK_SIGNAL_FUNC(entry_handler), info);
            gtk_widget_set_usize(all_long_options[opt].entry, 108,22);
            gtk_table_attach_defaults( GTK_TABLE( entry_table ), all_long_options[opt].entry, 0, 1, row, row + 1 );
            gtk_widget_show(all_long_options[opt].entry);

            GtkWidget * button = gtk_button_new_with_label ("min");
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                                GTK_SIGNAL_FUNC (long_options_min_callback), (gpointer) &all_long_options[opt] );
            gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 0, 1, row, row + 1 );
            gtk_widget_show (button);

            button = gtk_button_new_with_label ("def");
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                                GTK_SIGNAL_FUNC (long_options_def_callback), (gpointer) &all_long_options[opt] );
            gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 1, 2, row, row + 1 );
            gtk_widget_show (button);

            button = gtk_button_new_with_label ("max");
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                                GTK_SIGNAL_FUNC (long_options_max_callback), (gpointer) &all_long_options[opt] );
            gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 2, 3, row, row + 1 );
            gtk_widget_show (button);
          }

      for( int opt=0;opt<LAST_STRING_OPTION;opt++ )
        if( all_string_options[opt].page == i )
          {
            row++;

            GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
            gtk_table_attach_defaults( GTK_TABLE( description_table ), internal_hbox, 0, 1, row, row + 1 );
            gtk_widget_show (internal_hbox);

            GtkWidget * label = gtk_label_new(all_string_options[opt].label.chars());
            gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, TRUE, 0);
            gtk_widget_show(label);

            all_string_options[opt].entry = gtk_entry_new_with_max_length(all_string_options[opt].max_letters_in_entry);

            gtk_entry_set_text( GTK_ENTRY( all_string_options[opt].entry ), String(all_string_options[opt].value).chars() );

            entry_t * info = new entry_t( ENTRY_CHAR, false );

            gtk_signal_connect(GTK_OBJECT(all_string_options[opt].entry), "changed",
                               GTK_SIGNAL_FUNC(entry_handler), info);
            gtk_widget_set_usize(all_string_options[opt].entry, 108 ,22);
            gtk_table_attach_defaults( GTK_TABLE( entry_table ), all_string_options[opt].entry, 0, 1, row, row + 1 );
            gtk_widget_show(all_string_options[opt].entry);

            GtkWidget * button = gtk_button_new_with_label ("def");
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                                GTK_SIGNAL_FUNC (string_options_def_callback), (gpointer) &all_string_options[opt] );
            gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 1, 2, row, row + 1 );
            gtk_widget_show (button);
          }

      gtk_table_set_row_spacings( GTK_TABLE( description_table ) , 5 );
      gtk_table_set_col_spacings( GTK_TABLE( description_table ) , 5 );
      gtk_table_set_row_spacings( GTK_TABLE( entry_table ) , 5 );
      gtk_table_set_col_spacings( GTK_TABLE( entry_table ) , 5 );
      gtk_table_set_row_spacings( GTK_TABLE( button_table ) , 5 );
      gtk_table_set_col_spacings( GTK_TABLE( button_table ) , 5 );
      gtk_widget_show( description_table );
      gtk_widget_show( entry_table );
      gtk_widget_show( button_table );

      GtkWidget * label = gtk_label_new( page_titles[i] );
      gtk_notebook_append_page( GTK_NOTEBOOK( notebook ), page_vbox, label );
    }

  GtkWidget * hbox = gtk_hbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show (hbox);

  GtkWidget * button = gtk_button_new_with_label (" Default ");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (default_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label (" Load options ");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (load_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label (" Save options ");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (save_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label (" Save as default ");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (save_def_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label (" Apply ");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (apply_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label (" Ok ");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (ok_options_requested), (gpointer) NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label (" Cancel ");
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
ok_options_requested(GtkWidget * widget, gpointer data)
{
  the_opts.set_all_options_from_gui();
  the_opts.close_options_window();
}

void
load_options_requested(GtkWidget * widget, gpointer data)
{
  if(the_opts.get_filesel_widget() == NULL)
    {
      GtkWidget* filesel = gtk_file_selection_new( "Choose an options file" );
      gtk_signal_connect (GTK_OBJECT (filesel), "destroy",
                          (GtkSignalFunc) destroy_options_filesel, GTK_OBJECT(filesel));
      gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->ok_button),
                          "clicked", (GtkSignalFunc) load_filesel_ok_selected, filesel );
      gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->cancel_button),
                                 "clicked", (GtkSignalFunc) destroy_options_filesel,
                                 GTK_OBJECT (filesel));
      gtk_widget_show(filesel);
      the_opts.set_filesel_widget(filesel);
    }
}

void
load_filesel_ok_selected (GtkWidget * widget, GtkFileSelection * fs)
{
  the_opts.read_options_file(gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)),false);
  the_opts.update_all_gtk_entries();
  destroy_options_filesel();
}

void
save_options_requested(GtkWidget * widget, gpointer data)
{
  if(the_opts.get_filesel_widget() == NULL)
    {
      GtkWidget* filesel = gtk_file_selection_new( "Choose an options file" );
      gtk_signal_connect (GTK_OBJECT (filesel), "destroy",
                          (GtkSignalFunc) destroy_options_filesel, GTK_OBJECT(filesel));
      gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->ok_button),
                          "clicked", (GtkSignalFunc) save_filesel_ok_selected, filesel );
      gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (filesel)->cancel_button),
                                 "clicked", (GtkSignalFunc) destroy_options_filesel,
                                 GTK_OBJECT (filesel));
      gtk_widget_show(filesel);
      the_opts.set_filesel_widget(filesel);
    }
}

void
save_filesel_ok_selected (GtkWidget * widget, GtkFileSelection * fs)
{
  the_opts.save_all_options_to_file(gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)),false);
  destroy_options_filesel();
}

void
save_def_options_requested(GtkWidget * widget, gpointer data)
{
  the_opts.save_all_options_to_file("",true);
}

void
destroy_options_filesel()
{
  gtk_widget_destroy(the_opts.get_filesel_widget());
  the_opts.set_filesel_widget(NULL);
}

void
default_options_requested(GtkWidget * widget, gpointer data)
{
  the_opts.revert_all_options_to_default();
}

void
options_window_requested(GtkWidget * widget, gpointer data)
{
  if((the_arena.get_game_mode() == Arena::COMPETITION_MODE &&
      (the_arena.get_state() == NO_STATE ||
       the_arena.get_state() == NOT_STARTED ||
       the_arena.get_state() == FINISHED)) ||
     (the_arena.get_game_mode() != Arena::COMPETITION_MODE))
    {
      if(the_opts.get_options_window_up() == false)
        the_opts.setup_options_window();
      else
        the_opts.close_options_window();
    }
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
