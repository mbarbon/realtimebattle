In this chapter it is described how to use the program itself. If you find it boring to read it
through, feel free to use trial and error as usual, and return here if you stumble upon something
confusing in the user interface. It is, however, a good idea to read the short section on 
(!link [command line options] [CommandLineOptions]). Note also that there is no built-in help in the
program - here is where you should search for help.

!label CommandLineOptions
!subnode Command line options

In the current version, RealTimeBattle has only three command line options, all of which determine
in which competition-mode the program will run: (!T)debug(!t), (!T)normal(!t) or (!T)competition(!t) mode.

Usage:

(!T) RealTimeBattle (!t) (!I)[options](!i)

Options:
!begin_itemize
   !item (!T)  -d  (!t)  debug mode
   !item (!T)  -n  (!t)  normal mode  (default)
   !item (!T)  -c  (!t)  competition mode
!end_itemize

The differences between the three modes are shown in the following table

!begin_table [|l|cccccc|] !hline
   !! Debug     !! Pause !! Step    !! Robots may   !! Robots   !! Robot
   Mode !! command   !! in    !! through !! start child- !! may open !! CPU time
   !! available !! game  !! game    !! processes    !! files    !! unlimited
   !hline
   Debug       !! Yes !! Yes !! Yes !! Yes !! Yes !! Yes
   Normal      !! No  !! Yes !! No  !! No  !! No  !! Yes
   Competition !! No  !! No  !! No  !! No  !! No  !! No
   !hline
!end_table

!label ControlWindow
!subnode Control window

!begin_description
   !item [New tournament:] 
         This will start a new tournament. Section (!link [Start new tournament window] [TournamentWindow])
         will give more information.
   !item [Pause:] 
         Pauses for game, in (!link [(!T)competition-mode(!t)] [CommandLineOptions]) the pausing will be
         postponed until the end of the current game.  
   !item [Step:]
         In a paused game this will move one time step forward. It is intended to simplify debugging of
         the robots and is only available in (!link [(!T)debug-mode(!t)] [CommandLineOptions]). 
   !item [End:]
         Stops the current tournament.
   !item [Options:]
         Brings up the (!link [options window] [OptionsWindow]). 
   !item [Statistics:]
         Displays the (!link [statistics window] [StatisticsWindow]).
   !item [Quit:]
         Terminates the program.
!end_description

!label TournamentWindow
!subnode Start new tournament window

To select robots and arenas for the tournament, mark the files to the right and press the
(!T)add(!t) button. Selected files are shown to the left and can be removed correspondingly.

!label NumberOfGames

A (!link [tournament][Tournament]) consists of a number of 
(!link [sequences][Sequence]) of (!link [games][Game]). In each sequence the same robots are
playing in all games. Here you select the number of games and sequences as well as the number of
robots in each sequence. If you intend to play with all robots every game you are advised to choose
only one sequence and instead increase the number of game. The reason is to avoid restarting of the
robot processes which can take a wail, especially if the number of robots is large. 

You have to select at least two robots and one arena to be able to start.

!label ArenaWindow
!subnode Arena window

This is where the battle takes place. If you want to watch the game i more detailed, use the zoom
buttons! 

!label ScoreWindow
!subnode Score window

In this window the robots playing in the current sequence are listed. 

!label MessageWindow
!subnode Message window

Here are messages sent by the robots using (!link [Print and Debug][Print]) displayed.

!label OptionsWindow
!subnode Options window

Here you can change a number of options. In the (!link [Options chapter][Options]) you can get
detailed information on each option. The changes are not applied until the (!T)apply(!t) button is
pressed.

You can choose in to save your file in the (!link [options savefile][OptionsSavefile]).

!label StatisticsWindow
!subnode Statistics window

You can study the statistics of the current tournament in some different ways. You can either
display 

!begin_itemize
   !item statistics on individual (!B)Robot(!b)s,
   !item the result of a (!B)game(!b),
   !item the (!B)sequence total(!b) or
   !item the (!B)total(!b) of the tournament.
!end_itemize

Pressing the arrowed buttons will move you to the first, back one, forward one or to the last
elemant respectively. The middle bar will show what is display and pressing this will update the
statistics if the game is in progress.


