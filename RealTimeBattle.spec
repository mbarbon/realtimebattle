Summary: RealTimeBattle, a robot programming game for Unix
Name: RealTimeBattle
Version: #VERSION#
Release: #RELEASE#
Copyright: GPL
Group: X11/Games
Source: RealTimeBattle.tar.gz
URL: http://realtimebattle.netpedia.net

%description
RealTimeBattle is a programming game, in which robots controlled by
programs are fighting each other. The goal is to destroy the enemies,
using the radar to examine the environment and the cannon to shoot.

%prep
%setup -n RealTimeBattle

%build
./configure --enable-rpm="$RPM_OPT_FLAGS"
make

%install
make install
%post
install-info --entry "* RealTimeBattle: (RealTimeBattle).             A robot programming game" --info-dir /usr/info /usr/info/RealTimeBattle.info.gz
%preun
install-info --delete --info-dir /usr/info /usr/info/RealTimeBattle.info.gz

%files
%doc BUGS COPYING FAQ ChangeLog README TODO 
%doc Documentation/RealTimeBattle.dvi 
%doc Documentation/RealTimeBattle.html
%doc Documentation/RealTimeBattle-1.html
%doc Documentation/RealTimeBattle-2.html
%doc Documentation/RealTimeBattle-3.html
%doc Documentation/RealTimeBattle-4.html
%doc Documentation/RealTimeBattle-5.html
%doc Documentation/RealTimeBattle-6.html
%doc Documentation/prev.gif
%doc Documentation/next.gif
%doc Documentation/toc.gif

%dir /usr/games/RealTimeBattle
%dir /usr/games/RealTimeBattle/Arenas
%dir /usr/games/RealTimeBattle/Robots
%dir /usr/games/RealTimeBattle/Robots/empty
%dir /usr/games/RealTimeBattle/Robots/seek_and_destroy
%dir /usr/games/RealTimeBattle/Robots/rotate_and_fire

/usr/bin/RealTimeBattle
/usr/info/RealTimeBattle.info.gz

/usr/games/RealTimeBattle/RealTimeBattle.xpm
/usr/games/RealTimeBattle/Messagetypes.h
/usr/include/Messagetypes.h

/usr/games/RealTimeBattle/Arenas/Circle.arena
/usr/games/RealTimeBattle/Arenas/Circles.arena
/usr/games/RealTimeBattle/Arenas/Cross.arena
/usr/games/RealTimeBattle/Arenas/Diamond.arena
/usr/games/RealTimeBattle/Arenas/Forest.arena
/usr/games/RealTimeBattle/Arenas/Labyrinth.arena
/usr/games/RealTimeBattle/Arenas/Nippon.arena
/usr/games/RealTimeBattle/Arenas/Rooms.arena
/usr/games/RealTimeBattle/Arenas/Square.arena
/usr/games/RealTimeBattle/Arenas/Star.arena
/usr/games/RealTimeBattle/Arenas/Triangle.arena

/usr/games/RealTimeBattle/Robots/Makefile
/usr/games/RealTimeBattle/Robots/empty/empty.c
/usr/games/RealTimeBattle/Robots/empty/Makefile
/usr/games/RealTimeBattle/Robots/rotate_and_fire/rotate_and_fire.cc
/usr/games/RealTimeBattle/Robots/rotate_and_fire/Makefile
/usr/games/RealTimeBattle/Robots/seek_and_destroy/seek_and_destroy.cc
/usr/games/RealTimeBattle/Robots/seek_and_destroy/Makefile

/usr/games/RealTimeBattle/Robots/empty.robot
/usr/games/RealTimeBattle/Robots/seek_and_destroy.robot
/usr/games/RealTimeBattle/Robots/rotate_and_fire.robot
