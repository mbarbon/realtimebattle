#
# spec file for package RealTimeBattle (Version 0.9.11)
# 
# Author: CW Zuckschwerdt <zany@triq.net>, Nov 1999 
#

%define prefix  /usr
%define name    RealTimeBattle
%define version 1.0.0
%define release 1

Summary:     RealTimeBattle, a robot programming game for Unix
Name:        %{name}
Version:     %{version}
Release:     %{release}
Copyright:   GPL
Group:       X11/Games
Source:      http://www.lysator.liu.se/realtimebattle/Download/%{name}-%{version}.tar.gz
URL:         http://www.lysator.liu.se/realtimebattle
BuildRoot:   /tmp/%{name}-%{version}
Packager:    CW Zuckschwerdt <zany@triq.net>


%description
RealTimeBattle is a programming game, in which robots controlled by
programs are fighting each other. The goal is to destroy the enemies,
using the radar to examine the environment and the cannon to shoot.

%prep
%setup

%build
./configure --prefix=%{prefix} --enable-rpm="$RPM_OPT_FLAGS"
make

%install
make prefix=$RPM_BUILD_ROOT%{prefix} install

%post
/sbin/install-info /usr/info/RealTimeBattle.info.gz /usr/info/dir --entry="* RealTimeBattle: (RealTimeBattle).             A robot programming game."

%preun
if [ $1 = 0 ]; then
    /sbin/install-info --delete /usr/info/RealTimeBattle.info.gz /usr/info/dir --entry="* RealTimeBattle: (RealTimeBattle).             A robot programming game."
fi

%files

%doc ABOUT-NLS BUGS COPYING CVS-Help FAQ ChangeLog README TODO
%doc Documentation/*.html
%doc Documentation/*.gif
%doc Documentation/*.dvi

%{prefix}/bin/*
%{prefix}/include/*
%{prefix}/share/*
%{prefix}/info/*

%{prefix}/games/RealTimeBattle/*.h
%{prefix}/games/RealTimeBattle/*.xpm
%{prefix}/games/RealTimeBattle/*.txt
%{prefix}/games/RealTimeBattle/Arenas/*
%{prefix}/games/RealTimeBattle/Robots/*