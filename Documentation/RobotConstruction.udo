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

!subnode Messages to robots 
!label MessagesToRobots

!begin_description
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


!item [GameOption [optionnr (int)!] [value (double)!]]

!item [GameStarts]


!item [Radar [distance (double)!] [observed object type (int)!] [radar angle (double)!]]
# [energy level (int)!]] 

!item [Info [time (double)!] [speed (double)!] [cannon angle (double)!]]

!item [Energy [energy level!]]

!item [RobotsLeft [number of robots (int)!]]

!item [Collision [colliding object type (int)!] [angle relativ robot (double)!]]

!item [Warning [warning type (int)!] [message (string)!]]

!item [Dead]

!item [GameFinishes]

!item [SaveData]

!item [ExitRobot]

!end_description


!subnode Messages from robots
!label MessagesFromRobots  

!begin_description

!label RobotOption
!item [RobotOption [option nr (int)!] [value (int)!]]

!label Name
!item [Name [name (string)!]]

!label Colour
!item [Colour [home colour (hex)!] [away colour (hex)!]]

!label LoadData
!item [LoadData [type (int)!]]


!item [Rotate [what to rotate (int)!] [angular velocity (double)!]]

!item [RotateTo [what to rotate (int)!] [angular velocity (double)!] [end angle (double)!]] 

!item [RotateAmount [what to rotate (int)!] [angular velocity (double)!] [angle (double)!]] 

!item [Sweep [what to rotate (int)!] [angular velocity (double)!] [left angle (double)!] [right angle (double)!]] 

!item [Accelerate [value (double)!]]

!item [Break [percentage (double)!]]

!item [Shoot [shot energy (double)!]]

!item [Print [message (string)!]]

!item [Debug [message (string)!]]

!item [BinData [number of bytes (int)!] [binary data!]]

!item [AsciiData [data (string)!]]

!item [SaveDataFinished]

!end_description

