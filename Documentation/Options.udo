
RealTimeBattle is configurable with a bunch of options, collected in differnt groups. The philosophy
is to give you full freedom to set up the game in the way you like. This does mean, however,
that some settings of the options may give bad combination, which can cause troubles for the
program. 

!subnode Environmental options
!label EnvironmentalOptions

!begin_description
!item [Gravitational Constant:] The acceleration due to gravitation. It is about 9.8 on the
earth. An increase will increase the friction, thereby slowing robots down.

!item [Air resistance:] As it sounds. Increases with speed.

!item [Roll friction:] The friction in the direction of the robot if not breaking.

!item [Slide friction:] The friction orthogonal to the robot direction. Also the maximum friction if
breaking.

!end_description

!subnode Robot options
!begin_description
!item [Robot max acceleration:] Robots are not allowed to accelerate faster than this and !..
!item [Robot min acceleration:] slower than this. 
!label RobotRadius
!item [Robot radius:] Determines the size of the robot. 
!item [Robot mass:] Large robot mass will increase the impact of collisions.
!item [Robot bounce coefficient:] Affects how well the robots will bounce.
!item [Robot hardness coefficient:] Determines how servere injuries the robots will get when
colliding, this will by multiplied by the !..
!item [Robot protection coefficient:] to get how much to reduce the robot energy.
!item [Robot frontszie:] The front of the robot is a section with different materials, usually
harder and more protective, so robots can injure each other by ramming. 
!item [Robot front bounce coefficient:] See previous four items.
!item [Robot front hardness coefficient:] See previous five items.
!item [Robot front protection coefficient:] See previous six items.
!item [Robot start energy:] The amount of energy the robots will have at the beginning of each game.
!item [Robot max energy:] By eating cookie, the robot can increase its energy; not more than this,
though.
!item [Robot max rotate speed:] How fast the robot itself may rotate. Unit: radians/s .
!item [Robot cannon max rotate speed:] Maximum cannon rotate speed. Note that the cannon and the
radar move relative to the robot, so the actual rotation speed may be higher.
!item [Robot max rotate speed:] Maximum radar rotate speed. See note above.
!item [Robot energy levels:] The robot will only know its energy approximately. This will decide how
many discretation levels will be used.
!end_description


!subnode Shot options
!label ShotOptions

!begin_description
!item [Shot radius:] Size of shots. Should be less than (!link [robot radius][RobotRadius]).
!item [Shot speed:] Shots are moving in this speed in the direction of the cannon plus the velocity
of the robot.
!item [Shooting penalty:] When shooting the robot itself get damaged. This is the factor, by which
the shot energy is multiplied, to get the damaging energy. If the number of robots is large, this
number is reduced, so that you never lose in average by shooting (if you hit).
!item [Shot min energy:] The lowest shot energy allowed. A robot trying to shoot with less energy
will fail to shoot.
!item [Shot max energy:] The robots have a shot energy, which increase with time, but will never
exceed this value.
!item [Shot energy increase speed:] Determines how fast the robots shot energy noted above, will
increase. Unit: energy/s .
!end_description


!subnode Extras options
!label ExtrasOptions

!begin_description
!item [Cookie max energy:] The cookie energy is a random number between cookie max energy and cookie
min energy.
!item [Cookie min energy:] See above.
!item [Cookie frequency:] The number of cookie per second that will appear in average.
!item [Cookie radius:] Size of cookie. 
!item [Mine max energy:] The mine energy is a random number between mine max energy and mine
min energy.
!item [Mine min energy:] See above.
!item [Mine frequency:] The number of mine per second that will appear in average.
!item [Mine radius:] Size of mine. 
!item [Cookie colour:] Cookie colour in hexadecimal red--green--blue form.
!item [Mine colour:] As above.
!end_description


!subnode Time options
!label TimeOptions

!begin_description
!item [Max timestep:] If the computer is temporarily slowed down, the time between updates can be
to long. Setting this option will make the program artificially slow down the clock in those cases
and therefore violate the realtimeness.
!item [Time scale:] Increasing time scale than one means that the game clock will go faster relative
to an ordinary clock. Changing this value can be usefull if you either want to give the robots more
time or if you have a fast computer you may want to speed the game up.
!item [Start CPU time:]  In (!link [(!T)competition-mode(!t)][CommandLineOptions]) a robots CPU
usage is limited. At the beginning of a sequence a robot will get this amount of CPU time to
spent.
!item [Extra CPU time:] When the start CPU time is spended, the robot will get this amount of extra
CPU time.  
!item [Extra CPU period:] The extra CPU time must last the an entire CPU period, otherwise the robot
will die in the current game. 
!item [CPU time warning percentage:] When the robot has used up this amount of its CPU time it will
receive a warning message.
!item [Process check interval:] In (!link [(!T)competition-mode(!t)][CommandLineOptions]) this will
decide how often the program will check for CPU usage.
!end_description


!subnode Miscellaneous options
!label MiscellaneousOptions

!begin_description
!item [Arena scale:] Overall scale of the arena. A value of 2 give double sidelength, i.e. four
times bigger area.
!item [Background colour:] Background colour and !..
!item [Foreground colour:] foreground colour for the arena.
!item [Statistics savefile:] File to store (!link [statistics][StatisticsWindow]).
!item [Options savefile:] File to store (!link [options][OptionsWindow]).
!end_description

