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

string grabDigit(string jbyte, int place=1){
	stringstream str;
	string val; 
	if(jbyte.length()>1){
		if(place==10){
			str << jbyte.at(0);
			str >> val;
			str.str(" ");
			return val; 
		}
		if(place==1){
			str << jbyte.at(1);
			str >> val;
			str.str(" ");
			return val;
		}
	}else{
		if(place==10){
			str << "0";
			str >> val;
			str.str(" ");
			return val; 
		}
		if(place==1){
			str << jbyte.at(0);
			str >> val;
			str.str(" ");
			return val;
		}
	}
	return "f";
}

unsigned char bytize(string character){
	stringstream str;
	unsigned char val;
	str << character;
	str >> hex >> val;
	str.str(" ");
	return val;
}

int incrUp(int num){
	return num+1;
}

int main(int argc, char* argv[])
{
	// Argument Handling
	//string ip = string(argv[1]);
	char *ip = argv[1];
	int port_number=atoi(argv[2]);

	// TCP Support and Setup
	struct hostent *hp = gethostbyname( (const char*)ip ); if(!hp){ cout<<"unknown host"<<endl; return -1; }
	int sock = socket( PF_INET,SOCK_STREAM,IPPROTO_TCP); if(sock<0){ cout<<"socket not open"<<endl; return -1; }
	struct sockaddr_in name;
	socklen_t nlen = sizeof name;
	bzero( &name, nlen );
	bcopy( hp->h_addr, &name.sin_addr, hp->h_length );
	name.sin_family = AF_INET;
	name.sin_port = htons( port_number );

	// Joystick Support and Setup
	ifstream jbittys;
	jbittys.open("/dev/input/js0", ios::binary );
	unsigned char bit;
	string bitstring;
	stringstream stream;

	// Joystick Data Packet Calibration
	int numbutton=0;
	int numaxis=0;
	int numbytes=0;
	int packetcount=0;
	int idnum=0;
	string stridnum="";
	int typesel=0;
	cout << "How many buttons do you have?" << endl;
	cin >> numbutton;
	cout << "How many axises do you have?" << endl;
	cin >> numaxis;
	numbytes=numbutton*9+numaxis*9;
	//cout << "number of bytes to send: " << numbytes << endl;
	unsigned char packet[numbytes];
	for(int iii=0;iii<numbytes;iii++){
		packet[iii]=0;
	}
	unsigned char actuator[8];

	// TCP Connection Armed
	connect( sock, (sockaddr*)&name, nlen);

	// Infinite Joystick TCP Data Sending
	int bitcount=0;
	int arraycount=0;
	int actcount=0;
	int actDone=0;
	while(1){
		// Actuator Data Loading from Device
		stream.str(" ");
		stream << hex << jbittys.get();
		bitstring = stream.str();
		//if(bitcount==0){ cout << "timeA: 0x" << bitstring << " " <<  endl; }
		//if(bitcount==1){ cout << "timeB: 0x" << bitstring << " " << endl; }
		//if(bitcount==2){ cout << "timeC: 0x" << bitstring << " " << endl; }
		//if(bitcount==3){ cout << "idk: 0x" << bitstring << " " << endl; }
		if(bitcount==4){
			//cout << "bits(0-7): 0x" << bitstring << " " << endl;
			actuator[2]=bytize( grabDigit(bitstring,10) );
			actuator[3]=bytize( grabDigit(bitstring,1) );
		}
		if(bitcount==5){
			//cout << "bits(8-15): 0x" << bitstring << " " << endl;
			actuator[0]=bytize( grabDigit(bitstring,10) );
			actuator[1]=bytize( grabDigit(bitstring,1) );
		}
		if(bitcount==6){
			//cout << "type: 0x" << bitstring << " " << endl;
			actuator[4]=bytize( grabDigit(bitstring,10) );
                        actuator[5]=bytize( grabDigit(bitstring,1) );
		}
		if(bitcount==7){
			//cout << "id: 0x" << bitstring << " " << endl;
			actuator[6]=bytize( grabDigit(bitstring,10) );
                        actuator[7]=bytize( grabDigit(bitstring,1) );
			actDone=1;
		}
		bitcount=bitcount+1;
		// Packet Loading
		if(actDone==1){ 
			//cout << endl; 
			bitcount=0;
			actDone=0;
			stringstream str;
			//cout << "Actuator Bytes Collected: " << endl;
			for(int i=0;i<8;i++){
				//cout << i << " byte: " << hex << actuator[i] << endl;
				str << hex << actuator[5];
				str >> typesel;
				//cout << "typetypetype" << typesel << endl;
				str.str(" ");
				str.clear();
				//typesel=actuator[5];
				//cout << "typesel: " << typesel << endl;
				str << hex << actuator[7];
				str >> idnum;
				//cout << "idnumidnumidnum" << idnum << endl;
				str.str(" ");
				str.clear();
				//cout << hex << actuator[i] << endl;		//debug line
				//cout << "typesel: " << typesel << endl ;	//debug line	
				// Packet Location per sensor type
				if(typesel==2){ //axis
					//cout << "axis active start bit: " << idnum*9 + i*1 << " for ID: " << idnum << endl;
					packet[ idnum*9 + i*1 ]=actuator[i];
					//cout << hex << actuator[i] << " " ;
				}
				if(typesel==1){ //button
					//cout << "button active start bit: " << numaxis*9 + idnum*9 + i*9 << " for ID: " << idnum << endl;
                                        packet[ numaxis*9 + idnum*9 + i*1 ]=actuator[i];
					//cout << hex << actuator[i] << " " ;
                                }
				//actuator[i]=0;
				//cout << endl;
			}
			//cout << endl;
			//cout << "Done" << endl;
			// Show Packet
			cout << "entire packet: ";
			for(int ii=0; ii<numbytes; ii++){
				cout << packet[ii];
			}
			cout << endl;
			// Transmit Packet
			write(sock, packet, numbytes);
		}
	}

	// Closing Devices
	close( sock );
	jbittys.close();
	return 0;
}
