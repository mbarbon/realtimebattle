In this chapter it is described how to use the program itself. If you find it boring to read it
through, feel free to use trial and error as usual, and return here if you stumble upon something
confusing in the user interface. It is, however, a good idea to read the short section on 
(!link [command line options] [CommandLineOptions]). Note also that there is no built-in help in the
program - here is where you should search for help.

!subnode Command line options
!label CommandLineOptions
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

!subnode Start new tournament window
!label TournamentWindow

To select robots and arenas for the tournament, mark the files to the right and press the
(!T)add(!t) button. Selected files are shown to the left and can be removed correspondingly.

A tournament consists of a number of sequences of games. In each sequence the same robots are
playing in all games.

You have to select at least two robots and one arena to be able to start.


!subnode Options window
!label OptionsWindow

Here you can change a number of options. In the (!link [Options chapter][Options]) you can get
detailed information on each option. The changes are not applied until the (!T)apply(!t) button is
pressed.

You can choose in which file to save your options in (!T)Misc - file to save the options(!t). 

!subnode Statistics window
!label StatisticsWindow

You can study the statistics of the current tournament in some different ways. You can either
display 

!begin_itemize
   !item statistics on individual (!T)Robot(!t)s,
   !item the result of a (!T)game(!t),
   !item the (!T)sequence total(!t) or
   !item the (!T)total(!t) of the tournament.
!end_itemize

Pressing the arrowed buttons will move you to the first, back one, forward one or to the last
elemant respectively. The middle bar will show what is display and pressing this will update the
statistics if the game is in progress.


