/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "GuiRequest.h"
#include "Various.h"
#include "ArenaController.h"
#include "OptionHandler.h"
#include "EventHandler.h"
#include "Tournament.h"

#include "ServerSocket.h"

//TODO : change this one to TournamentAgreementPackets
#include "ServerPackets.h"
#include "TournamentAgreementPackets.h"

extern EventHandler the_eventhandler;

void
QuitProgramRequest::accept() const
{
    Quit( success ); // TODO: How should we quit really?
}

void
OptionChangeRequest::accept() const
{
  the_opts.set_option( option, string_value, as_default );
}

void
TogglePauseGameRequest::accept() const
{
  //TODO: toggle pause game
}

void
StartTournamentRequest::accept() const
{
  cout<<"void StartTournamentRequest::accept()\n";
  the_eventhandler.set_tournament( new Tournament ( my_tournament_info ) );
  /*
  the_eventhandler.set_tournament( new Tournament( robots_per_match,
                                                   number_of_matches,
                                                   robot_files,
                                                   arena_files ) );
  */
}

void
EndTournamentRequest::accept() const
{
  //TODO: end tournament
}

void
ChangeDebugLevelRequest::accept() const
{
  //TODO: change debug level
}

void
BroadCastTournamentChangeRequest::accept() const
{
  TournamentCommitChangePacket P(type + " " + value);
  my_socketserver.send_packet_by_type(TOURN_AGREE_CHANNEL, &P);
}

void
OpenTournamentAgreementChannelRequest::accept() const
{
  if(create_channel) {
    my_socketserver.open_channel(&my_tournament_agreement_packetfactory);
  }
  else {
    my_socketserver.open_channel(&my_tournament_agreement_packetfactory);
    CommandPacket C("NeedFriends " + my_tournament_agreement_packetfactory.Protocol());
    my_socketserver.send_packet_by_type(SERVER_CONNECTION, &C );
  }
}
