Summary: RealTimeBattle, a robot programming game for Unix
Name: RealTimeBattle
Version: #VERSION#
Release: #RELEASE#
Copyright: GPL
Group: X11/Games
Source: RealTimeBattle.tar.gz
URL: TODO: Get homepage

%description


%prep
%setup -n RealTimeBattle

%build
make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
make rpm-install

%files
%doc BUGS COPYING FAQ NEWS README TODO 
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

/usr/bin/RealTimeBattle
/usr/info/RealTimeBattle.info.gz


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

/usr/games/RealTimeBattle/Robots/empty.robot
/usr/games/RealTimeBattle/Robots/fire.robot
/usr/games/RealTimeBattle/Robots/thomastest.robot
/usr/games/RealTimeBattle/Robots/thomas2.robot
