
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#ifdef TIME_WITH_SYS_TIME 
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <Messagetypes.h>

volatile bool quit_robot = false;

message_to_robot_type name2msg_to_robot_type(char* msg_name);

class RotateAndFire
{
public:
  RotateAndFire               ();
  ~RotateAndFire              () {}

  // From robot

  void robot_option           ( const int option_nr, const int value );
  void name                   ( const char* name );
  void colour                 ( const char* home, const char* away );
  void rotate                 ( const int what, const double vel );
  void rotate_to              ( const int what, const double vel,
                                const double angle );
  void rotate_amount          ( const int what, const double vel,
                                const double angle );
  void sweep                  ( const int what, const double vel,
                                const double left, const double right );
  void accelerate             ( const double amount );
  void brake                  ( const double amount );
  void shoot                  ( const double energy );
  void print                  ( const char* message );
  void debug                  ( const char* message );
  void debug_line             ( const double start_angle,
                                const double start_radius,
                                const double end_angle,
                                const double end_radius );
  void debug_circle           ( const double center_angle,
                                const double center_radius,
                                const double circle_radius );

  // To robot

  void initialize             ( const int first_seq );
  void your_name              ( const char* prev_name );
  void your_colour            ( const char* colour );
  void game_option            ( const int option, const double value );
  void game_starts            ();
  void radar_noobject         ( const double dist, const double angle );
  void radar_robot            ( const double dist, const double angle );
  void radar_shot             ( const double dist, const double angle );
  void radar_wall             ( const double dist, const double angle );
  void radar_cookie           ( const double dist, const double angle );
  void radar_mine             ( const double dist, const double angle );
  void info                   ( const double time, const double speed,
                                const double cannon_angle );
  void robot_info             ( const double energy, int enemy );
  void rotation_reached       ( const int what );
  void energy                 ( const double energylevel );
  void robots_left            ( const int number_of_robots );
  void collision_noobject     ( const double angle );
  void collision_robot        ( const double angle );
  void collision_shot         ( const double angle );
  void collision_wall         ( const double angle );
  void collision_cookie       ( const double angle );
  void collision_mine         ( const double angle );
  void warning                ( const int type, const char* message );
  void dead                   ();
  void game_finishes          ();
  void exit_robot             ();

  // generic functions

  void pre_checking_messages  ();
  static void check_messages  ( int sig );

private:

  double robot_rotate;
  double acceleration;
};

// Declare the class_object

class RotateAndFire raf_obj;


RotateAndFire::RotateAndFire()
{
  robot_rotate = 0.53;
  acceleration = 0.54;

  robot_option( SIGNAL, SIGUSR1 );
}

//
// Messages from robot
//

void
RotateAndFire::robot_option( const int option, const int value )
{
  cout << "RobotOption " << option << " " << value << endl;
}

void
RotateAndFire::name( const char* name )
{
  cout << "Name " << name << endl;
}

void
RotateAndFire::colour( const char* home, const char* away )
{
  cout << "Colour " << home << " " << away << endl;
}

void
RotateAndFire::rotate( const int what, const double vel )
{
  cout << "Rotate " << what << " " << vel << endl;
}

void
RotateAndFire::rotate_to( const int what, const double vel,
                          const double angle )
{
  cout << "RotateTo " << what << " " << vel << " " << angle << endl;
}

void
RotateAndFire::rotate_amount( const int what, const double vel,
                              const double angle )
{
  cout << "RotateAmount " << what << " " << vel << " " << angle << endl;
}

void
RotateAndFire::sweep( const int what, const double vel,
                      const double left, const double right )
{
  cout << "Sweep " << what << " " << vel << " "
       << left << " " << right << endl;
}

void
RotateAndFire::accelerate( const double amount )
{
  cout << "Accelerate " << amount << endl;
}

void
RotateAndFire::brake( const double amount )
{
  cout << "Brake " << amount << endl;
}

void
RotateAndFire::shoot( const double energy )
{
  cout << "Shoot " << energy << endl;
}

void
RotateAndFire::print( const char* message )
{
  cout << "Print " << message << endl;
}

void
RotateAndFire::debug( const char* message )
{
  cout << "Debug " << message << endl;
}

void
RotateAndFire::debug_line( const double start_angle,
                           const double start_radius,
                           const double end_angle,
                           const double end_radius )
{
  cout << "DebugLine " << start_angle << " " << start_radius
       << " " << end_angle << " " << end_radius << endl;
}

void
RotateAndFire::debug_circle( const double center_angle,
                             const double center_radius,
                             const double circle_radius )
{
  cout << "DebugCircle " << center_angle << " " << center_radius 
       << " " << circle_radius << endl;
}

//
// Messages to robot
//

void
RotateAndFire::initialize( const int first_seq )
{
  if( first_seq == 1 )
    {
      name( "Rotate and Fire v2" );
      colour( "ffaaaa", "aaaaff" );
    }
}

void
RotateAndFire::your_name( const char* prev_name )
{
}

void
RotateAndFire::your_colour( const char* colour )
{
}

void
RotateAndFire::game_option( const int option, const double value )
{
}

void
RotateAndFire::game_starts()
{
  rotate( 1, robot_rotate );
  rotate( 6, 1.5 );
  accelerate( acceleration );
}

void
RotateAndFire::radar_noobject( const double dist, const double angle )
{
}

void
RotateAndFire::radar_robot( const double dist, const double angle )
{
  shoot( 2 );
}

void
RotateAndFire::radar_shot( const double dist, const double angle )
{
}

void
RotateAndFire::radar_wall( const double dist, const double angle )
{
  double old_acc = acceleration;
  if( dist < 0.6 )
    acceleration = 0.0;
  else
    acceleration = 1.0;

  if( old_acc != acceleration )
    accelerate( acceleration );
}

void
RotateAndFire::radar_cookie( const double dist, const double angle )
{
}

void
RotateAndFire::radar_mine( const double dist, const double angle )
{
}

void
RotateAndFire::info( const double time, const double speed,
                     const double cannon_angle )
{
}

void
RotateAndFire::robot_info( const double energy, int enemy )
{
}

void
RotateAndFire::rotation_reached( const int what )
{
}

void
RotateAndFire::energy( const double energylevel )
{
}

void
RotateAndFire::robots_left( const int number_of_robots )
{
}

void
RotateAndFire::collision_noobject( const double angle )
{
}

void
RotateAndFire::collision_robot( const double angle )
{
}

void
RotateAndFire::collision_shot( const double angle )
{
}

void
RotateAndFire::collision_wall( const double angle )
{
}

void
RotateAndFire::collision_cookie( const double angle )
{
  print( "Cookie eaten!" );
}

void
RotateAndFire::collision_mine( const double angle )
{
  print( "Oh no! A mine" );
}

void
RotateAndFire::warning( const int type, const char* message )
{
  char full_message[200];

  switch( type )
    {
    case UNKNOWN_MESSAGE:
      strcpy( full_message, "Unknown message: " );
      break;
    case PROCESS_TIME_LOW:
      strcpy( full_message, "Process time low: " );
      break;
    case MESSAGE_SENT_IN_ILLEGAL_STATE:
      strcpy( full_message, "Message sent in illegal state: " );
      break;
    case UNKNOWN_OPTION:
      strcpy( full_message, "Unknown option: " );
      break;
    case OBSOLETE_KEYWORD:
      strcpy( full_message, "Obsolete Keyword: " );
      break;
    }

  strcat( full_message, message );
  print( full_message );
}

void
RotateAndFire::dead()
{
  print( "Too damaged, giving up." );
}

void
RotateAndFire::game_finishes()
{
}

void
RotateAndFire::exit_robot()
{
  print( "Shutting down and leaving" );
  quit_robot = true;
}

// generic functions

void
RotateAndFire::pre_checking_messages()
{
  if(rand() < (RAND_MAX/100))
    {
      robot_rotate = - robot_rotate;
      rotate( 1, robot_rotate );
    }
}

void
RotateAndFire::check_messages(int sig)
{
  quit_robot = false;

  char msg_name[81];
  message_to_robot_type msg_t;

  raf_obj.pre_checking_messages();

  cin.clear();
  while( !cin.eof() )
    {
      cin >> msg_name;
      msg_t = name2msg_to_robot_type(msg_name);
      switch(msg_t)
        {
        case INITIALIZE:
          {
            int init;
            cin >> init;
            raf_obj.initialize( init );
          }
          break;
        case YOUR_NAME:
          {
            char name[81];
            cin >> name;
            raf_obj.your_name( name );
          }
          break;
        case YOUR_COLOUR:
          {
            char col[81];
            cin >> col;
            raf_obj.your_colour( col );
          }
          break;
        case GAME_STARTS:
          raf_obj.game_starts();
          break;

        case RADAR:
          {
            double dist, angle;
            int object;

            cin >> dist >> object >> angle;
            switch(object)
              {
              case NOOBJECT:
                raf_obj.radar_noobject( dist, angle );
                break;
              case ROBOT:
                raf_obj.radar_robot( dist, angle );
                break;
              case WALL:
                raf_obj.radar_wall( dist, angle );
                break;
              case SHOT:
                raf_obj.radar_shot( dist, angle );
                break;
              case COOKIE:
                raf_obj.radar_cookie( dist, angle );
                break;
              case MINE:
                raf_obj.radar_mine( dist, angle );
                break;
              default:
                cout << "Print Unknown Object seen!" << endl;
                break;
              }
          }
          break;
        case COLLISION:
          {
            int object;
            double angle;

            cin >> object >> angle;
            switch( object )
              {
              case NOOBJECT:
                raf_obj.collision_noobject( angle );
                break;
              case ROBOT:
                raf_obj.collision_robot( angle );
                break;
              case WALL:
                raf_obj.collision_wall( angle );
                break;
              case SHOT:
                raf_obj.collision_shot( angle );
                break;
              case COOKIE:
                raf_obj.collision_cookie( angle );
                break;
              case MINE:
                raf_obj.collision_mine( angle );
                break;
              default:
                cout << "Print Collided with Unknown Object!" << endl;
                break;
              }
          }
          break;
        case WARNING:
          {
            int type;
            char text[81];
            cin >> type;
            cin.getline(text,80,'\n');
            raf_obj.warning( type, text );
          }
          break;
        case EXIT_ROBOT:
          raf_obj.exit_robot();
          break;
        default:
          break;
        }
    }
  signal (sig, check_messages);
}

message_to_robot_type
name2msg_to_robot_type(char* msg_name)
{
  for(int i=0; message_to_robot[i].msg[0] != '\0'; i++)
    {
      if( strcmp(message_to_robot[i].msg, msg_name) == 0 )
        return (message_to_robot_type)i;
    }
  return UNKNOWN_MESSAGE_TO_ROBOT;
}


int 
main(int argc, char * argv[])
{
  sigset_t usr1set;

  signal(SIGUSR1, RotateAndFire::check_messages);

  // libpthread seems to block USR1 sometimes for some reason
  sigemptyset(&usr1set);
  sigaddset(&usr1set, SIGUSR1);
  sigprocmask(SIG_UNBLOCK, &usr1set, NULL);

  timeval current_time;
  gettimeofday(&current_time, NULL);
  srand(current_time.tv_usec);

  for(;;sleep(1))
    if( quit_robot ) 
      return( EXIT_SUCCESS );
}
