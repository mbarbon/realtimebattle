#include "SocketHandler.h"

void
SocketHandler::SocketHandler() {};

void
SocketHandler::~SocketHandler() {};

void
SocketHandler::check_socket() {
	struct timeval tv;
	tv.tv_sec  = 0;
	tv.tv_usec = 0; //500000;

     	FD_ZERO( &readfs );
        FD_ZERO( &exceptfs );

	set_fd();
	
	if( select( max_desc + 1, &readfs, NULL, &exceptfs, &tv ) < 0 )
	{
		cout<< "select failed." << endl;
		quit();
	}

	check_fd();
}

