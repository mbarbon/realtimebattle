!label Game
!subnode A game

At the start of a game, the robots are given a random position on the arena, with random
orientation. The radar and the cannon are both pointing forwards and the
(!link [shot energy][ShotEnergy]) is set at zero. The goal for the robots is now to survive as long
as possible and at the same time to destroy the other robots. A robot will get one point for each
of the enemy robot it outlives. One extra point is, however, given to all the participating
robots. Robots dying at the same time will get the mean value of the points they would have got if
they didn't die simultaneously (or in other word, they get half a point for each of the other dying
at the same time).

A game is finished either if the number of living robots is less than two or the 
(!link [time is up][Timeout]).  

!label Sequence
!subnode A sequence

A sequence is a series of games, where the same robots are fighting. At the beginning of a sequence
the robot processes are started. The number of robots in a sequence is limited to 120, due to the
limit on 256 open filedescriptors in linux. For every robot two pipes are opened as communication
channels to the robot.  

Thereafter a (!link [number of games][NumberOfGames]) are
played and finally the robot processes are killed.

!label Tournament
!subnode A tournament

A tournament is a collection of sequences. 

The number of robots in a tournament is (theoretically) unlimited.


Any number of sequences is allowed, but to make the tournament fair, you should choose a number of
sequences, such that all robots will play the same number of games (i.e. #sequence = #robots per
game / GCD( #robots per game, #robots in the tournament).

