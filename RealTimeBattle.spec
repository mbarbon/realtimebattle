#
# spec file for package RealTimeBattle (Version 0.9.11)
# 
# Author: CW Zuckschwerdt <zany@triq.net>, Nov 1999 
#

%define prefix  /usr
%define name    RealTimeBattle
%define version 0.9.11
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
# %post
# install-info --entry "* RealTimeBattle: (RealTimeBattle).             A robot programming game" --info-dir /usr/info /usr/info/RealTimeBattle.info.gz
# %preun
# install-info --delete --info-dir /usr/info /usr/info/RealTimeBattle.info.gz

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

%{prefix}/
