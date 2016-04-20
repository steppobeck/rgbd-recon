/* dtrack: receives and processes DTrack udp packets (ASCII protocol), A.R.T. GmbH 25.8.00-s.u.
 * Common code tested under Linux, Irix, Windows NT 4.0, Windows 2000, Windows XP
 *
 * Version v0.3.4
 *
 * Purpose:
 *   - receives DTrack udp packets (ASCII protocol) and converts them into easier to handle data
 *   - sends DTrack remote commands (udp)
 *   - DTrack network protocol due to: 'Technical Appendix DTrack v1.18 (July 7, 2003)'
 *   - for DTrack versions v1.16 - v1.20 (and compatible versions)
 *
 * Usage:
 *   - for Linux, Irix: just compile
 *   - for Windows NT 4.0, Windows 2000, Windows XP:
 *      - comment '#define OS_UNIX', uncomment '#define OS_WIN'
 *      - link with 'ws2_32.lib'
 *
 * Versions:
 * v0.1 (25.8.00, KA): ASCII-Format
 * v0.2 (10.3.01, KA): 'neues' Datenformat
 *   p1 (10.4.01, KA): Linux und NT
 *   p2 (26.9.01, KA): Protokoll-Erweiterung fuer DTrack v1.16.1: '6dcal'
 * v0.3 (15.10.01, KA): Protokoll-Erweiterung fuer DTrack v1.16.2: '6df'
 *   p1 (11.1.02, KA): 'remote'-Schnittstelle
 *   p2 (29.1.03, KA): Timestamp-Erweiterung fuer DTrack v1.17.3
 *   p3 (25.9.03, KA): Protokoll-Erweiterung fuer DTrack v1.18.0: '6dmt'
 *   p4 (26.5.04, KA): udp_receive erweitert (OS-Buffer komplett leeren)
 *
 * $Id: dtrack.c++,v 1.1 2006/10/27 15:10:22 beck3 Exp $
 */

#define OS_UNIX   1    // for Linux, Irix
//#define OS_WIN    1    // for Windows NT 4.0, Windows 2000, Windows XP

// --------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef OS_UNIX
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <unistd.h>
#endif
#ifdef OS_WIN
	#include <windows.h>
	#include <winsock.h>
#endif

#include "dtrack.h"

// Constants dependent from protocol:

#define PROT_MAX_LINES   8  // maximum number of lines in a udp packet



// --------------------------------------------------------------------------
// Constructor & Destructor

DTrack::DTrack()
	: _udpsock(-1),
	  _udptimeout_us(1000000),
	  _udpbufsize(10000),
	  _udpbuf(NULL),
	  _remote_ip(0),
	  _remote_port(0)
{
}

DTrack::~DTrack()
{
}


// --------------------------------------------------------------------------
// Initialization:
// ini (i): parameters
// return value (o): error code

int DTrack::init(dtrack_init_type* ini)
{

	// init udp socket:

	_udpsock = udp_init(ini->udpport);
	if(_udpsock < 0){
		return DTRACK_ERR_UDP;
	}

	_udptimeout_us = ini->udptimeout_us;

	// init udp buffer:

	_udpbufsize = ini->udpbufsize;
	
	_udpbuf = (char *)malloc(_udpbufsize);
	if(!_udpbuf){
		udp_exit(_udpsock);
		return DTRACK_ERR_MEM;
	}

	// DTrack remote control parameters:

	_remote_ip = udp_inet_atoh(ini->remote_ip);
	_remote_port = ini->remote_port;

	return DTRACK_ERR_NONE;
}


// --------------------------------------------------------------------------
// Exit:
// return value (o): error code

int DTrack::exit(void)
{

	// exit udp buffer:

	if(_udpbuf){
		free(_udpbuf);
	}

	// exit udp socket:

	if(_udpsock > 0){
		if(udp_exit(_udpsock)){
			return DTRACK_ERR_UDP;
		}
	}

	return DTRACK_ERR_NONE;
}


// --------------------------------------------------------------------------
// Receive and process DTrack udp packet (ASCII protocol):

// framenr (o): frame counter
// timestamp (o): timestamp (-1, if information not available in packet)

// nbodycal (o): number of calibrated bodies (-1, if information not available in packet)
// nbody (o): number of tracked bodies
// body (o): array containing 6d data
// max_nbody (i): maximum number of bodies in array body (no processing is done, if 0)

// nflystick (o): number of calibrated flysticks
// flystick (o): array containing 6df data
// max_nflystick (i): maximum number of flysticks in array flystick (no processing is done, if 0)

// nmeatool (o): number of calibrated measurement tools
// meatool (o): array containing 6dmt data
// max_nmeatool (i): maximum number of measurement tools in array (no processing is done, if 0)

// nmarker (o): number of tracked single markers
// marker (o): array containing 3d data
// max_nmarker (i): maximum number of marker in array marker (no processing is done, if 0)

// return value (o): error code

int DTrack::receive_udp_ascii(
	unsigned long* framenr, double* timestamp,
	int* nbodycal, int* nbody, dtrack_body_type* body, int max_nbody,
	int* nflystick, dtrack_flystick_type* flystick, int max_nflystick,
	int* nmeatool, dtrack_meatool_type* meatool, int max_nmeatool,
	int* nmarker, dtrack_marker_type* marker, int max_nmarker
){
	char* strs[PROT_MAX_LINES];
	char* s;
	int iline, nlines;
	int i, len, n;
	unsigned long ul, ularr[2];

	// Defaults:

	*framenr = 0;
	*timestamp = -1;   // i.e. not available
	*nbodycal = -1;    // i.e. not available
	*nbody = 0;
	*nflystick = 0;
	*nmeatool = 0;
	*nmarker = 0;

	// Receive udp packet:

	len = udp_receive(_udpsock, _udpbuf, _udpbufsize, _udptimeout_us);

	if(len == -1){
		return DTRACK_ERR_TIMEOUT;
	}
	if(len <= 0){
		return DTRACK_ERR_UDP;
	}

	// Split packet in lines:

	if((nlines = split_lines(_udpbuf, len, strs, PROT_MAX_LINES)) == 0){
		return DTRACK_ERR_PCK;
	}

	// Process lines:

	for(iline=0; iline<nlines; iline++){
		s = strs[iline];

		// Line for frame counter:

		if(!strncmp(s, "fr ", 3)){
			s += 3;
			
			if(!(s = get_ul(s, framenr))){       // get frame counter
				*framenr = 0;
				return DTRACK_ERR_PCK;
			}
			continue;
		}

		// Line for timestamp:

		if(!strncmp(s, "ts ", 3)){
			s += 3;
			
			if(!(s = get_d(s, timestamp))){      // get timestamp
				*timestamp = 0;
				return DTRACK_ERR_PCK;
			}
			continue;
		}
		
		// Line for additional information about number of calibrated bodies:

		if(!strncmp(s, "6dcal ", 6)){
			if(max_nbody <= 0){
				continue;
			}
			
			s += 6;
			
			if(!(s = get_ul(s, &ul))){            // get number of bodies
				return DTRACK_ERR_PCK;
			}

			*nbodycal = (int )ul;
			continue;
		}

		// Line for 6d data:

		if(!strncmp(s, "6d ", 3)){
			if(max_nbody <= 0){
				continue;
			}
			
			s += 3;
			
			if(!(s = get_ul(s, &ul))){            // get number of bodies
				return DTRACK_ERR_PCK;
			}

			*nbody = n = (int )ul;
			if(n > max_nbody){
				n = max_nbody;
			}

			for(i=0; i<n; i++){                  // get data of body
				if(!(s = get_block(s, "uf", &body[i].id, &body[i].quality))){
					return DTRACK_ERR_PCK;
				}
				
				if(!(s = get_block(s, "ffffff", NULL, body[i].loc))){
					return DTRACK_ERR_PCK;
				}
				
				if(!(s = get_block(s, "fffffffff", NULL, body[i].rot))){
					return DTRACK_ERR_PCK;
				}
			}
			
			continue;
		}
		
		// Line for flystick data:

		if(!strncmp(s, "6df ", 4)){
			if(max_nflystick <= 0){
				continue;
			}
			
			s += 4;
			
			if(!(s = get_ul(s, &ul))){            // get number of flysticks
				return DTRACK_ERR_PCK;
			}

			*nflystick = n = (int )ul;
			if(n > max_nflystick){
				n = max_nflystick;
			}

			for(i=0; i<n; i++){                  // get data of body
				if(!(s = get_block(s, "ufu", ularr, &flystick[i].quality))){
					return DTRACK_ERR_PCK;
				}
				flystick[i].id = ularr[0];
				flystick[i].bt = ularr[1];
				
				if(!(s = get_block(s, "ffffff", NULL, flystick[i].loc))){
					return DTRACK_ERR_PCK;
				}
				
				if(!(s = get_block(s, "fffffffff", NULL, flystick[i].rot))){
					return DTRACK_ERR_PCK;
				}
			}
			
			continue;
		}
		
		// Line for measurement tool data:

		if(!strncmp(s, "6dmt ", 5)){
			if(max_nmeatool <= 0){
				continue;
			}
			
			s += 5;
			
			if(!(s = get_ul(s, &ul))){            // get number of flysticks
				return DTRACK_ERR_PCK;
			}

			*nmeatool = n = (int )ul;
			if(n > max_nmeatool){
				n = max_nmeatool;
			}

			for(i=0; i<n; i++){                  // get data of body
				if(!(s = get_block(s, "ufu", ularr, &meatool[i].quality))){
					return DTRACK_ERR_PCK;
				}
				meatool[i].id = ularr[0];
				meatool[i].bt = ularr[1];
				
				if(!(s = get_block(s, "fff", NULL, meatool[i].loc))){
					return DTRACK_ERR_PCK;
				}
				
				if(!(s = get_block(s, "fffffffff", NULL, meatool[i].rot))){
					return DTRACK_ERR_PCK;
				}
			}
			
			continue;
		}
		
		// Line for single markers:

		if(!strncmp(s, "3d ", 3)){
			if(max_nmarker <= 0){
				continue;
			}
			
			s += 3;
			
			if(!(s = get_ul(s, &ul))){            // get number of markers
				return DTRACK_ERR_PCK;
			}

			*nmarker = n = (int )ul;
			if(n > max_nmarker){
				n = max_nmarker;
			}

			for(i=0; i<n; i++){                  // get marker data
				if(!(s = get_block(s, "uf", &marker[i].id, &marker[i].quality))){
					return DTRACK_ERR_PCK;
				}

				if(!(s = get_block(s, "fff", NULL, marker[i].loc))){
					return DTRACK_ERR_PCK;
				}
			}
			
			continue;
		}

		// ignore invalid line identifier
	}

	return DTRACK_ERR_NONE;
}


// ---------------------------------------------------------------------------------------------------
// Send remote control command (udp) to DTrack:

// cmd (i): command code
// val (i): additional value (if needed)
// return value (o): error code

int DTrack::send_udp_command(unsigned short cmd, int val)
{
	char cmdstr[100];

	if(!_remote_ip){
		return DTRACK_ERR_CMD;
	}

	// process command code:

	switch(cmd){
		case DTRACK_CMD_CAMERAS_OFF:
			strcpy(cmdstr, "dtrack 10 0");
			break;
			
		case DTRACK_CMD_CAMERAS_ON:
			strcpy(cmdstr, "dtrack 10 1");
			break;
			
		case DTRACK_CMD_CAMERAS_AND_CALC_ON:
			strcpy(cmdstr, "dtrack 10 3");
			break;
			
		case DTRACK_CMD_SEND_DATA:
			strcpy(cmdstr, "dtrack 31");
			break;
			
		case DTRACK_CMD_STOP_DATA:
			strcpy(cmdstr, "dtrack 32");
			break;
			
		case DTRACK_CMD_SEND_N_DATA:
			sprintf(cmdstr, "dtrack 33 %d", val);
			break;
			
		default:
			return DTRACK_ERR_CMD;
	}

	// send udp packet:

	if(udp_send(_udpsock, cmdstr, strlen(cmdstr) + 1, _remote_ip, _remote_port, _udptimeout_us)){
		return DTRACK_ERR_CMD;
	}

	if(cmd == DTRACK_CMD_CAMERAS_AND_CALC_ON){
		#ifdef OS_UNIX
			sleep(1);
		#endif
		#ifdef OS_WIN
			Sleep(1000);
		#endif
	}

	return DTRACK_ERR_NONE;
}


// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------

// Split packet in single lines (substitute cr/lf through '\0'):
// str (i): packet
// len (i): length of packet (in bytes)
// strarr (o): array of pointers to the first character of the lines within str
// maxlines (i): length of strarr
// return value (o): number of found lines

int DTrack::split_lines(char* str, unsigned long len, char** strarr, int maxlines)
{
	unsigned long i = 0;
	int index = 0;
	char* s = str;

	while(*str && (i<len)){
		if(*str == '\r'){         // substitute cr
			*str = '\0';
		}else if(*str == '\n'){   // lf: end of line found
			*str = '\0';
			
			strarr[index++] = s;
			if(index == maxlines){  // stop processing, if maximum number reached
				return index;
			}
			s = str + 1;
		}
		str++;
		i++;
	}

	return index;
}


// Read next 'unsigned long' value from string:
// str (i): string
// ul (o): read value
// return value (o): pointer behind read value in str; NULL in case of error

char* DTrack::get_ul(char* str, unsigned long* ul)
{
	char* s;
	
	*ul = strtoul(str, &s, 0);
	return (s == str) ? NULL : s;
}


// Read next 'double' value from string:
// str (i): string
// d (o): read value
// return value (o): pointer behind read value in str; NULL in case of error

char* DTrack::get_d(char* str, double* d)
{
	char* s;
	
	*d = strtod(str, &s);
	return (s == str) ? NULL : s;
}


// Read next 'float' value from string:
// str (i): string
// f (o): read value
// return value (o): pointer behind read value in str; NULL in case of error

char* DTrack::get_f(char* str, float* f)
{
	char* s;
	
	*f = (float )strtod(str, &s);   // strtof() only available in GNU-C
	return (s == str) ? NULL : s;
}


// Process next block '[...]' in string:
// str (i): string
// fmt (i): format string ('u' for 'unsigned long', 'f' for 'float')
// uldat (o): array for 'unsigned long' values (long enough due to fmt)
// fdat (o): array for 'float' values (long enough due to fmt)
// return value (o): pointer behind read value in str; NULL in case of error

char* DTrack::get_block(char* str, char* fmt, unsigned long* uldat, float* fdat)
{
	char* strend;
	int index_ul, index_f;

	if(!(str = strchr(str, '['))){       // search begin of block
		return NULL;
	}
	if(!(strend = strchr(str, ']'))){    // search end of block
		return NULL;
	}
	str++;
	*strend = '\0';

	index_ul = index_f = 0;

	while(*fmt){
		switch(*fmt++){
			case 'u':
				if(!(str = get_ul(str, &uldat[index_ul++]))){
					return NULL;
				}
				break;
				
			case 'f':
				if(!(str = get_f(str, &fdat[index_f++]))){
					return NULL;
				}
				break;
				
			default:    // ignore unknown format character
				break;
		}
	}

	return strend + 1;
}


// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------

// Initialize udp socket:
// port (i): port number
// return value (o): socket number, <0 if error

int DTrack::udp_init(unsigned short port)
{
   int sock;
   struct sockaddr_in name;

	// init socket dll (only NT):

	#ifdef OS_WIN
	{
		WORD vreq;
		WSADATA wsa;

		vreq = MAKEWORD(2, 0);
		if(WSAStartup(vreq, &wsa) != 0){
			return -1;
		}
	}
	#endif
        
   // create the socket:

	sock = socket(PF_INET, SOCK_DGRAM, 0);
   if (sock < 0){
      return -2;
   }

   // name the socket:

	name.sin_family = AF_INET;
   name.sin_port = htons(port);
   name.sin_addr.s_addr = htonl(INADDR_ANY);
   if (bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0){
      return -3;
   }
        
   return sock;
}


// Deinitialize udp socket:
// sock (i): socket number
// return value (o): 0 ok, -1 error

int DTrack::udp_exit(int sock)
{
	int err;

	#ifdef OS_UNIX
		err = close(sock);
	#endif
	
	#ifdef OS_WIN
		err = closesocket(sock);
		WSACleanup();
	#endif

	if(err < 0){
		return -1;
	}

	return 0;
}


// Receiving udp data:
// sock (i): socket number
// buffer (o): buffer for udp data
// maxlen (i): length of buffer
// tout_us (i): timeout in us (micro sec)
// return value (o): number of received bytes, <0 if error/timeout occured

int DTrack::udp_receive(int sock, void *buffer, int maxlen, unsigned long tout_us)
{
	int nbytes, err;
	fd_set set;
	struct timeval tout;

	// waiting for data:

	FD_ZERO(&set);
	FD_SET(sock, &set);

	tout.tv_sec = tout_us / 1000000;
	tout.tv_usec = tout_us % 1000000;

	switch((err = select(FD_SETSIZE, &set, NULL, NULL, &tout))){
		case 1:
			break;
		case 0:
			return -1;    // timeout
		default:
	      return -2;    // error
	}

	// receiving packet:

	while(1){

		// receive one packet:

		nbytes = recv(sock, (char *)buffer, maxlen, 0);

		if(nbytes < 0){  // receive error
			return -3;
		}

		// check, if more data available: if so, receive another packet
		
		FD_ZERO(&set);
		FD_SET(sock, &set);

		tout.tv_sec = 0;   // no timeout
		tout.tv_usec = 0;

		if(select(FD_SETSIZE, &set, NULL, NULL, &tout) != 1){
			
			// no more data available: check length of received packet and return
			
			if(nbytes >= maxlen){   // buffer overflow
      		return -4;
		   }

			return nbytes;
		}
	}
}


// Sending udp data:
// sock (i): socket number
// buffer (i): buffer for udp data
// len (i): length of buffer
// ipaddr (i): ip address to send to
// port (i): port number to send to
// tout_us (i): timeout in us (micro sec)
// return value (o): 0 if ok, <0 if error/timeout occured

int DTrack::udp_send(int sock, void* buffer, int len, unsigned long ipaddr, unsigned short port, unsigned long tout_us)
{
	fd_set set;
	struct timeval tout;
	int nbytes, err;
	struct sockaddr_in addr;

	// building address:

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ipaddr);
	addr.sin_port = htons(port);

	// waiting to send data:

	FD_ZERO(&set);
	FD_SET(sock, &set);

	tout.tv_sec = tout_us / 1000000;
	tout.tv_usec = tout_us % 1000000;

	switch((err = select(FD_SETSIZE, NULL, &set, NULL, &tout))){
		case 1:
			break;
		case 0:
			return -1;    // timeout
		default:
	      return -2;    // error
	}

	// sending data:

	nbytes = sendto(sock, (char* )buffer, len, 0, (struct sockaddr* )&addr, (size_t )sizeof(struct sockaddr_in));

	if(nbytes < len){  // send error
      return -3;
   }

	return 0;
}


// Converting string to ip address:
// s (i): string
// return value (o): ip address, 0 if error occured

unsigned long DTrack::udp_inet_atoh(char* s)
{
	int i, a[4];
	char* s1;
	unsigned long ret;
	
	s1 = s;
	while(*s1){
		if(*s1 == '.'){
			*s1 = ' ';
		}
		s1++;
	}

	if(sscanf(s, "%d %d %d %d", &a[0], &a[1], &a[2], &a[3]) != 4){
		return 0;
	}

	ret = 0;
	for(i=0; i<4; i++){
		if(a[i] < 0 || a[i] > 255){
			return 0;
		}
		
		ret = (ret << 8) | (unsigned char)a[i];
	}

	return ret;
}
