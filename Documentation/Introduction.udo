

RealTimeBattle is a programming game, in which robots controlled by the competitors programs are
fighting each other.  The goal is to destroy the opponents, using the radar to examine the
environment and the cannon to shoot.

Features include:

!begin_itemize
   !item Game progresses in real time, with the robot programs running as child processes to
         RealTimeBattle and communication with standard input and output.
   !item Robots can be construct in almost any programming language.
   !item Up to 120 robots competing simultaneously.
   !item A simple messaging language is used for communication, which makes easy to start
         constructing robots.
   !item Robots behave like real physical object.
   !item High configurability.
!end_itemize

!label Requirements
!subnode Requirements

The hardware requirements is very much dependent on what you do. Running a few robots should be
possible to do on any computer on which linux or any other Unixes can be run. The demand on hardware
will, however, grow the robots you want on the battlefield simultaneously; running 120 advanced
robots is certainly demanding for any personal computer.

RealTimeBattle is only available on Unix. It is developed on a linux-machine, but it should compile
on other Unixes too. Running in (!link [(!T)competition-mode(!t)][CommandLineOptions]) is currently
only possible in linux, because of the need to read the (!T)/proc(!t) directory for information on
CPU usage.

The only software requirement is (!xlink [gtk+][http://www.gtk.org]), which is used for the
graphical user interface.

!label Background
!subnode Background

The project started in August 1998.
Inspiration came from
(!xlink [RobotBattle] [http://www.robotbattle.com/]), a very interesting game, which however suffers from
two major drawbacks:

!begin_itemize
   !item It is only available on Windows.
   !item The robots are written in an own language, which restricts the possibilities to write
         intelligent robots severely.
!end_itemize

We therefore decided to construct a Unix-version, which makes use of features of a modern OS.


!label License
!subnode License

RealTimeBattle is copyrighted under the (!xlink [General Public License][http://???]), in the spirit of the 
(!xlink [Linux][http://www.linux.org]) community. Official versions of RealTimeBattle will,only be
released by the authors.

