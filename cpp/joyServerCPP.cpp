#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[])
{
	// Argument Handling
	int port_number=atoi(argv[1]);

	//TCP Support and Setup
	int bytecount=0;
	unsigned char joy[1024];
	int sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sock < 0){ return -1;}
	struct sockaddr_in	self;
	socklen_t		nlen = sizeof self;
	bzero( &self, sizeof self );
	self.sin_family = AF_INET;
	self.sin_port = htons( port_number );
	self.sin_addr.s_addr = htonl( INADDR_ANY );
	if ( bind( sock, (sockaddr *)&self, nlen ) < 0 ){return	-1;}
	if ( listen( sock, 5 ) < 0 ){return	-1;}		
	//while ( 1 )
	//		{
		sockaddr_in	peer;
		socklen_t	plen = sizeof peer;
		bzero( &peer, plen );
		printf( "server waiting \n" );
		int client = accept( sock, (sockaddr *)&peer, &plen );
		if ( client < 0 ) { perror( "accept" ); return -1;}  //break; }
	while(1){		
		bytecount=read( client, joy, 1024);
		if ( bytecount < 0 ) { perror( "read" ); break; }
		//cout << " client joustick data received " << endl;
		cout << "joystick packet: ";
		for(int i=0; i<bytecount;i++){
			cout << hex << joy[i] ;
		}
		cout << endl;
		}
	//close( client );
	close( sock );
	printf( "\n" );

}
