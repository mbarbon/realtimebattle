
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

const double pi = 3.1415927;

volatile bool quit_robot = false;

message_to_robot_type name2msg_to_robot_type(char* msg_name);

class RotateAndFire
{
public:
  // The constructor and destructor.
  RotateAndFire               ();
  ~RotateAndFire              () {}

  // These functions are equivalent to messages sent from the robot
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

  // These functions are called when the robot gets a message
  // of the corresponding type. There are several radar and
  // collision functions. These functions corresponds to each
  // object that the robot has collided with or found with the radar.
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

  // This function is called before checking messages.
  void pre_checking_messages  ();

  // This function goes through all messages and calls the
  // corresponding function whenever a message is recieved.
  static void check_messages  ( int sig );

private:

  // These variables contains the current values of the robot rotation,
  // acceleration and brake.
  double robot_rotate;
  double acceleration;
  double brake_value;

  // Boolean variables to control if it is allowed to change robot
  // rotation and acceleration.
  bool rotate_allowed;
  bool acceleration_change_allowed;

  // Variables that controls how frequent the robot has been hit by shots.
  int shots_hit;
  double last_shot_hit_time;
  int number_of_robots_left;

  // The current time is stored in this variable.
  double current_time;

  // All game options are stored in these variables.
  double robot_max_rotate;
  double robot_cannon_max_rotate;
  double robot_radar_max_rotate;
  double robot_max_acceleration;
  double robot_min_acceleration;
  double robot_start_energy;
  double robot_max_energy;
  double robot_energy_levels;
  double shot_speed;
  double shot_min_energy;
  double shot_max_energy;
  double shot_energy_increase_speed;
  double timeout;
  double debug_level;
};

// Declare the robot object

class RotateAndFire raf_obj;

// The constructor. Initialize standard values of robot_rotate,
// acceleration and brake_value.
// Robot options is also sent here.
// We want the SIGUSR1 signal to be sent to us.
// We want a message when RotateTo or RotateAmount is finished
RotateAndFire::RotateAndFire()
{
  robot_rotate = 0.53;
  acceleration = 0.54;
  brake_value = 0.0;

  robot_option( SIGNAL, SIGUSR1 );
  robot_option( SEND_ROTATION_REACHED, 1 );
}

// Function for sending messages from robot. 
// They do nothing but write the command and its arguments to cout.
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

// Here are all functions that is called whenever a message of
// corresponding type is recieved.

// Send name and colour if it is the first sequence.
void
RotateAndFire::initialize( const int first_seq )
{
  if( first_seq == 1 )
    {
      name( "Rotate and Fire v2" );
      colour( "ffaaaa", "aaaaff" );
    }
}

// The name the robot got in the game.
void
RotateAndFire::your_name( const char* prev_name )
{
}

// The colour the robot got in the game.
void
RotateAndFire::your_colour( const char* colour )
{
}

// This function is called when a game option is changed and
// the robot's copy of this option will change.
void
RotateAndFire::game_option( const int option, const double value )
{
  switch( option )
    {
    case ROBOT_MAX_ROTATE:
      robot_max_rotate = value;
      break;
    case ROBOT_CANNON_MAX_ROTATE:
      robot_cannon_max_rotate = value;
      break;
    case ROBOT_RADAR_MAX_ROTATE:
      robot_radar_max_rotate = value;
      break;
    case ROBOT_MAX_ACCELERATION:
      robot_max_acceleration = value;
      break;
    case ROBOT_MIN_ACCELERATION:
      robot_min_acceleration = value;
      break;
    case ROBOT_START_ENERGY:
      robot_start_energy = value;
      break;
    case ROBOT_MAX_ENERGY:
      robot_max_energy = value;
      break;
    case ROBOT_ENERGY_LEVELS:
      robot_energy_levels = value;
      break;
    case SHOT_SPEED:
      shot_speed = value;
      break;
    case SHOT_MIN_ENERGY:
      shot_min_energy = value;
      break;
    case SHOT_MAX_ENERGY:
      shot_max_energy = value;
      break;
    case SHOT_ENERGY_INCREASE_SPEED:
      shot_energy_increase_speed = value;
      break;
    case TIMEOUT:
      timeout = value;
      break;
    case DEBUG_LEVEL:
      debug_level = value;
      break;
    }
}

// Reset the robot when a new game starts. Send initial rotate and
// accelerate commands.
void
RotateAndFire::game_starts()
{
  robot_rotate = 0.53;
  acceleration = 0.54;
  brake_value = 0.0;

  rotate_allowed = true;
  shots_hit = 0;
  last_shot_hit_time = -2.0;
  current_time = 0;

  rotate( 1, robot_rotate );
  rotate( 6, robot_cannon_max_rotate - fabs(robot_rotate) - robot_rotate );
  accelerate( acceleration );
}

// Radar info when no object is seen, should never happen.
void
RotateAndFire::radar_noobject( const double dist, const double angle )
{
}

// Radar info when robot is seen.
// Lock cannon and radar on the seen robot,
// and if it is near enough, begin to brake. Shoot at it.
void
RotateAndFire::radar_robot( const double dist, const double angle )
{
  rotate( 6, -robot_rotate );
  if( dist < 2 )
    {
      acceleration = 0.0;
      accelerate( acceleration );
      brake_value = 1.0;
      brake( brake_value );
    }
  shoot( 2 );
}

// Radar info when a shot is seen.
void
RotateAndFire::radar_shot( const double dist, const double angle )
{
  rotate( 6, robot_cannon_max_rotate - fabs(robot_rotate) - robot_rotate );
}

// Radar info when a wall is seen.
// If a wall is near in the robot direction, begin to brake,
// else have normal acceleration.
void
RotateAndFire::radar_wall( const double dist, const double angle )
{
  rotate( 6, robot_cannon_max_rotate - fabs(robot_rotate) - robot_rotate );

  double old_acc = acceleration;

  double mod_angle = fmod( angle, 2 * pi ) - pi;

  if( mod_angle > -pi / 3 && mod_angle < pi / 3 )
    {
      if( dist < 1.0 )
        {
          acceleration = 0.0;
          brake_value = 1.0;
        }
      else
        {
          acceleration = 1.0;
          brake_value = 0.0;
        }
    }

  if( old_acc != acceleration )
    {
      accelerate( acceleration );
      brake( brake_value );
    }
}

// Radar info when a cookie is seen.
void
RotateAndFire::radar_cookie( const double dist, const double angle )
{
  rotate( 6, robot_cannon_max_rotate - fabs(robot_rotate) - robot_rotate );
}

// Radar info when a mine is seen.
void
RotateAndFire::radar_mine( const double dist, const double angle )
{
  rotate( 6, robot_cannon_max_rotate - fabs(robot_rotate) - robot_rotate );
  shoot( shot_min_energy );
}

// Get information about time, speed and cannon_angle
// Update current time and check how long time has passed since
// we were last hit by a shot.
void
RotateAndFire::info( const double time, const double speed,
                     const double cannon_angle )
{
  current_time = time;

  if( current_time - last_shot_hit_time > 2.0 )
    shots_hit = 0;
}

// Information about seen robots
void
RotateAndFire::robot_info( const double energy, int enemy )
{
}

// Check if rotation has been reached.
// If it is the robot. Begin normal acceleration and rotation
void
RotateAndFire::rotation_reached( const int what )
{
  if( what & 1 == 1 )
    {
      rotate_allowed = true;
      acceleration_change_allowed = true;
      int direction = rand() / (RAND_MAX/2);
      if( direction == 0 ) direction = -1;
      robot_rotate = 0.53 * direction;
      acceleration = 1.0;
      accelerate( acceleration );
    }
}

// Our energy level.
void
RotateAndFire::energy( const double energylevel )
{
}

// How many robots is left in the game.
void
RotateAndFire::robots_left( const int number_of_robots )
{
  number_of_robots_left = number_of_robots;
}

// We have collided with no object. Should not happen.
void
RotateAndFire::collision_noobject( const double angle )
{
}

// We have collided with another robot.
void
RotateAndFire::collision_robot( const double angle )
{
}

// We have collided with a shot.
// Set last_shot_hit_time and increase shots_hit.
// If shots_hit is too high, begin fleeing acceleration and rotation.

void
RotateAndFire::collision_shot( const double angle )
{
  shots_hit++;
  last_shot_hit_time = current_time;

  if( shots_hit > 5 )
    {
      int direction = rand() / (RAND_MAX/2);
      if( direction == 0 ) direction = -1;
      robot_rotate = robot_max_rotate * direction;
      rotate_amount( 1, robot_rotate, pi / 2 );
      rotate_allowed = false;
      acceleration = robot_max_acceleration;
      accelerate( acceleration );
      acceleration_change_allowed = false;
      shots_hit = 0;
      print( "Hit too many times! Flee!" );
    }
}

// We have collided with a wall.
void
RotateAndFire::collision_wall( const double angle )
{
}

// We have collided with a cookie.
void
RotateAndFire::collision_cookie( const double angle )
{
  print( "Cookie eaten!" );
}

// We have collided with a mine.
void
RotateAndFire::collision_mine( const double angle )
{
  print( "Oh no! A mine" );
}

// We have received a warning message.
// Print the message.
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

// Unfortunately we have died.
void
RotateAndFire::dead()
{
}

// The game has finished
void
RotateAndFire::game_finishes()
{
}

// You have been ordered to exit the robot
void
RotateAndFire::exit_robot()
{
  print( "Shutting down and leaving" );
  quit_robot = true;
}

// This function is called before checking messages.
// Every hundred time, change the direction of the rotation.
void
RotateAndFire::pre_checking_messages()
{
  if(rand() < (RAND_MAX/100) && rotate_allowed )
    {
      robot_rotate = -robot_rotate;
      rotate( 1, robot_rotate );
    }
}

// This function handles all the message checking and
// calls the appropriate function.
// This function is called whenever a SIGUSR1 (decided in the
// robotoption sent to the server) signal is received.
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
        case GAME_OPTION:
          {
            int nr;
            double value;
            cin >> nr >> value;
            raf_obj.game_option( nr, value );
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
        case INFO:
          {
            double time, speed, cannon_angle;
            cin >> time >> speed >> cannon_angle;
            raf_obj.info( time, speed, cannon_angle );
          }
          break;
        case ROBOT_INFO:
          {
            double energy;
            int enemy;
            cin >> energy >> enemy;
            raf_obj.robot_info( energy, enemy );
          }
          break;
        case ROTATION_REACHED:
          {
            int what;
            cin >> what;
            raf_obj.rotation_reached( what );
          }
          break;
        case ENERGY:
          {
            double en;
            cin >> en;
            raf_obj.energy( en );
          }
          break;
        case ROBOTS_LEFT:
          {
            int nr;
            cin >> nr;
            raf_obj.robots_left( nr );
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
        case DEAD:
          raf_obj.dead();
          break;
        case GAME_FINISHES:
          raf_obj.game_finishes();
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

// A function to convert the messagename to an enum message_to_robot_type
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


// The main function.
// Initialize the signal function and get a random seed based on current
// time in microseconds.
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
