This chapter will describe what you need to know in order to build your own robot. Most 
important to know is the messaging language, which is a set of about 35 commands used to communicate
with the server program. It is also instructive to study the example robots in the (!T)Robots/(!t)
directory. 

!subnode General information
!label GeneralInformation

#RealTimeBattle is designed to make use of the multitasking properties of unix-systems.

   At the beginning of each sequence the robot processes are launched by the server program and
   assigned two pipes, one for input and the other for output. These are connected to the (!T)stdin(!t)
   and (!T)stdout(!t), so that from the robots point of view, it is communtiating with the server via
   the standard input and output ports.

   This approach mean that the robots can be written in any programming languages. However, there is
   one catch, the robot must be able to know when it has received a message. One solution is to tell
   the server to (!link [send a signal][RobotOption]) after sending the message. This requires the
   robot program to be able to handle signals. This can be done in many languages including c, c++ and
   perl. Note that it is not a good idea to do "busy wait", i.e., to actively look for a message until
   you get one. This will slow things down considerably and, worse, in 
   (!link [(!T)competition-mode(!t)][CommandLineOptions]) the robot will rapidly run out of CPU-time
   and die.

!subnode messagetypes.h
!label messagetypes

The file (!T)messagetypes.h(!t) is a good source of information on the messaging language. It is a c/c++
include file, but you can easily rewrite it to use with other languages. There you can find listing
of messages, warning types, objects, game options and robot options.
   
!subnode Messages to robots 
!label MessagesToRobots
   
!begin_description
   !label Initialize
   !item [Initialize [first? (int)!]]
         This is the very first message the robot will get. If the argument is one, it is the first sequence
         in the tournament and it should send (!link [name][Name]) and (!link [colour][Colour]) to the
         server, otherwise it should wait for (!link [YourName][YourName]) and 
         (!link [YourColour][YourColour]) messages.
         
   !label YourName
   !item [YourName [name (string)!]]
         Current name of the robot, don't change it if you don't have very good reasons.

   !label YourColour
   !item [YourColour [colour (hex)!]]
         Current colour of the robot, change if you find it ugly.

   !label BinDataTo
   !item [BinData [number of bytes (int)!] [binary data!]]
         If you request to (!link [load data][LoadData]) in binary form, this message will be send to
         you. The number of bytes is less than 500.

   !item [AsciiData [data (string)!]]
         As above, but data in an ascii string. The string length is less than 500.

   !item [LoadDataFinished]
         Signals that all data has been transfered.


   !item [GameOption [optionnr (int)!] [value (double)!]] At the beginning of each game the robots
         will be sent a number of settings, which can be useful to the robot. For a complete list of
         these, look in the file (!T)messagetypes.h(!t) for the (!T)game_option_type(!t)
         enum. In the (!link [options chapter][Options]) you can get more detailed information on
         each option. 

   !item [GameStarts] This message is sent when the game starts (surprise!)
         
   !item [Radar [distance (double)!] [observed object type (int)!] [radar angle (double)!]]
# [energy level (int)!]] 
         This message given information each turn from the radar. Remember that the radar-angle is
         relative to the robot front; it is given in radians.
         
   !item [Info [time (double)!] [speed (double)!] [cannon angle (double)!]]
         The (!T)info(!t) message does always follow the (!T)radar(!t) message. It gives more
         general information on the state of the robot. The time is the game-time elapsed since the
         start of the game. This is not necessarily the same as the real time elapsed, due to 
         (!link [timescale][TimeScale]) and (!link [max timestep] [MaxTimestep]).

   !item [Energy [energy level!]]
         The end of each round the robot will get to know its energy level. It will not, however,
         get the exact energy, instead it is discretized into a number of 
         (!link [energy levels][EnergyLevels]).

   !item [RobotsLeft [number of robots (int)!]]
         At the beginning of the game and when a robot is killed the number of remaining robots is
         broadcasted to all living robots.

   !item [Collision [colliding object type (int)!] [angle relative robot (double)!]]
         When a robot hits (or is hit by) something it gets this message. In the file
         (!link [messagetypes.h][messagetypes]) you can find a list of the object types.
         
   !item [Warning [warning type (int)!] [message (string)!]]
         A warning message can be sent for various reasons
   !begin_description
      !item [Unknown message:] The server received a message it couldn't recognize. 
      !item [Process time low:] The CPU usage has reached the 
            (!link [CPU warning percentage][CPUWarningPercentage]). Only in 
            (!link [(!T)competition-mode(!t)][CommandLineOptions]).
      !item [Message sent in illegal state:] The message received couldn't be handled in this state
            of the program. For example (!link [Rotate][Rotate]) is sent before the game has
            started.
   !end_description

   !item [Dead]
         Robot died. Do not try to send more messages to the server until the end of the game, the
         server doesn't read them.

   !item [GameFinishes]
         Current game is finished, get prepared for the next!

   !item [SaveData]
         The current sequence is finished. If you want to store some data, do it now! When finished
         saving you should send a (!link [SaveDataFinished][SaveDataFinished]) message. Thereafter
         you will get the message !..

   !label ExitRobot
   !item [ExitRobot]
         Exit from the program immediately! Otherwise it will be killed forcefully.

!end_description


!subnode Messages from robots
!label MessagesFromRobots  

!begin_description

   !label RobotOption
   !item [RobotOption [option nr (int)!] [value (int)!]]
         Currently only one option is available, SEND_SIGNAL, which tells the server to send SIGUSR1
         when there is a message waiting. Send this message (with argument 0=true) as soon as you
         are  prepared to receive the signal. 

   !label Name
   !item [Name [name (string)!]]
         When receiving the (!link [Initialize][Initialize]) message with argument 1, indicating
         that this is the first sequence, you should send both your name and your colour.

   !label Colour
   !item [Colour [home colour (hex)!] [away colour (hex)!]]
         See above.

   !label LoadData
   !item [LoadData [type (int)!]]
         Tell to load stored data. Type of data: 0 - binary, 1 - ascii.

   !label Rotate
   !item [Rotate [what to rotate (int)!] [angular velocity (double)!]]
         Set the angular velocity for the robot, its cannon and/or its radar. Set 'what to rotate'
         to 1 for robot, 2 for cannon, 4 for radar or to a sum of these to rotate more objects at
         the same time. The angular velocity is given is radians per second and is limited by 
         (!link [Robot (cannon/radar) max rotate speed][RobotMaxRotateSpeed]). 

   !item [RotateTo [what to rotate (int)!] [angular velocity (double)!] [end angle (double)!]] 
         As (!B)rotate(!b), but will rotate to a given angle. Not that radar and cannon angles are
         relative to the robot angle.

   !item [RotateAmount [what to rotate (int)!] [angular velocity (double)!] [angle (double)!]] 
         As (!B)rotate(!b), but will rotate relative the current angle.

   !item [Sweep [what to rotate (int)!] [angular velocity (double)!] [left angle (double)!] [right angle (double)!]] 
         As (!B)rotate(!b), but sets the radar and/or the cannon (not available for the robot
         itself) in a sweep mode.

   !item [Accelerate [value (double)!]]
         Set the robot acceleration. Value is bounded by 
         (!link [Robot max/min acceleration][RobotMaxAcceleration]). 

   !item [Break [portion (double)!]]
         Set the break. Full break (portion = 1.0) means that the friction in the robot direction is
         equal to (!link [Slide friction][SlideFriction]).

   !item [Shoot [shot energy (double)!]]
         Shoot with the given energy. (!link [The shot options][ShotOptions]) give more information.

   !label Print
   !item [Print [message (string)!]]
         Print message on the (!link [message window] [MessageWindow]).

   !item [Debug [message (string)!]]
         Print message on the (!link [message window] [MessageWindow]) if in 
         (!link [(!T)debug mode(!t)] [CommandLineOptions]).

   !item [BinData [number of bytes (int)!] [binary data!]]
         Send data to be stored. Be sure to send the correct number of bytes!

   !item [AsciiData [data (string)!]]
         Send data to be store. The string is terminated by an endline charcter.

   !label SaveDataFinished
   !item [SaveDataFinished]
         Tell the server that saving is finished. The robot will thereafter get an 
         (!link [ExitRobot][ExitRobot]) message.
!end_description

