In RealTimeBattle, it is very simple to construct your own arenas. The language consists of only
eight commands, and there are essentially three building blocks: line, circle and
inner_circle. This is mainly because of speed reason, for circles and lines it is very easy to check
if a collision has accured. Circle and inner_circle prevents robots from entering into a circle or
getting outside 
a circle respectivly. The line stops the robots from entering through the long side, but no check is
made on the short side, so you have to put cicles at each end of the line to make it a solid
object. The commands (!T)polygon(!t) and (!T)closed_polygon(!t) are intended to simplify this
procedure, always giving a proper object.

Note that RealTimeBattle does no check on whether the arenafile gives a correct arena, it is
entirely up to you, however, it will complain if there are violations to the language rules.

Arena files should be given the postfix (!T).arena(!t) and be stored in the 
(!link [arena directory][ArenaDirectory]), for RealTimeBattle to find it.

The (!T)bounce coefficient(!t) and (!T)hardness(!t) argument given to all wall creating commands,
determines the material of the wall. They are both values betwween 0 and 1. Harder walls will injure
colliding robots more and higher bounce coefficient make them bounce better. 

You are also encouraged to study the arenas included and learn from the examples.

!subnode Command list

A command consists of the command name and the arguments separated by whitespace. Be sure to give
the right number of arguments!

In the command list, the arguments are given in square brackets.

!begin_description
   !item [scale [value!]]
         This value times the (!link [arena scale][ArenaScale]) will give the scale factor,
         by which all coordinates are multiplied. This command must be, if exists, the very first in
         the file, default is 1.0.

   !item [boundary [left!][up!][right!][down!]]
         The boundary encloses the area, in which robot, cookies and mines are placed. It also
         determines the visible area in the (!link [arena window][ArenaWindow]). This command is
         required and may only be preceded by (!T)scale(!t).

   !item [inner_circle [bounce!][hardness!][center_x!][center_y!][radius!]]
         Robots are limited to the inside of this circle.

   !item [circle [bounce!][hardness!][center_x!][center_y!][radius!]]
         Circle shaped wall.

   !item [polygon [bounce!][hardness!][thickness!][number of vertices!] ([center_x!][center_y!]!..)]
         This will create a number of circles, connected by lines. 

   !item [closed_polygon [bounce!][hardness!][thickness!][number of vertices!] ([center_x!][center_y!]!..)]
         As a (!T)polygon(!t), but the first and the last vertices are also connected by a line.

   !item [line [bounce!][hardness!][thickness!][start_x!][start_y!][end_x!][end_y!]]
         Creates a line. See the (!link [beginning of the chapter][ArenaConstruction]) for more informations. 

   !item [exclusion_point [position_x!][position_y!]]
         When the arena inside the boudary consists of several separated areas, you should exclude
         all but one by 
         inserting exclusion points. All points from which you can draw a straight line to an
         exlusion point without crossing a wall, are considered to be outside the arena.
         
!end_description