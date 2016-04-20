/* dtrack: receives and processes DTrack udp packets (ASCII protocol), A.R.T. GmbH 25.8.00-26.5.04
 * Common code for Unix/Linux/Windows NT/Windows 2000/Windows XP
 *
 * Version v0.3.4
 *
 * DTrack network protocol due to: 'Technical Appendix DTrack v1.18 (July 7, 2003)'
 * for DTrack versions v1.16 - v1.20 (and compatible versions)
 *
 * $Id: dtrack.h,v 1.1 2006/10/27 15:10:22 beck3 Exp $
 */

#ifndef _ART_DTRACK_H
#define _ART_DTRACK_H


// --------------------------------------------------------------------------
// Error codes:

#define DTRACK_ERR_NONE       0
#define DTRACK_ERR_UDP       -1   // error handling the udp socket
#define DTRACK_ERR_MEM       -2   // error handling the udp buffer
#define DTRACK_ERR_TIMEOUT   -3   // timeout while receiving data
#define DTRACK_ERR_CMD       -4   // error while sending remote command
#define DTRACK_ERR_PCK       -10  // error in udp packet


// --------------------------------------------------------------------------
// DTrack remote commands:

#define DTRACK_CMD_CAMERAS_OFF           0x1000
#define DTRACK_CMD_CAMERAS_ON            0x1001
#define DTRACK_CMD_CAMERAS_AND_CALC_ON   0x1003

#define DTRACK_CMD_SEND_DATA             0x3100
#define DTRACK_CMD_STOP_DATA             0x3200
#define DTRACK_CMD_SEND_N_DATA           0x3300


// --------------------------------------------------------------------------
// Initialize Lib

// Initialization parameters:

typedef struct{
	unsigned short udpport;        // udp port number
	int udpbufsize;                // size of buffer for udp packets (in bytes)
	unsigned long udptimeout_us;   // udp timeout (receiving and sending) in us (micro sec)

	char remote_ip[20];            // DTrack remote control: ip address of DTrack pc ("\0" if not used)
	unsigned short remote_port;    // port number of DTrack remote control (0 if not used)
} dtrack_init_type;


// --------------------------------------------------------------------------
// Receiving DTrack data

// Body data (6d):

typedef struct{
	unsigned long id;     // id number
	float quality;        // quality (0 <= qu <= 1)
	float loc[3];         // location (in mm)
	float ang[3];         // orientation angles (eta, theta, phi; in deg)
	float rot[9];         // rotation matrix (column-wise)
} dtrack_body_type;

// Flystick data (6d + buttons):

typedef struct{
	unsigned long id;     // id number
	float quality;        // quality (0 <= qu <= 1, no tracking if -1)
	unsigned long bt;     // pressed buttons (binary coded)
	float loc[3];         // location (in mm)
	float ang[3];         // orientation angles (eta, theta, phi; in deg)
	float rot[9];         // rotation matrix (column-wise)
} dtrack_flystick_type;

// Measurement tool data (6d + buttons):

typedef struct{
	unsigned long id;     // id number
	float quality;        // quality (0 <= qu <= 1, no tracking if -1)
	unsigned long bt;     // pressed buttons (binary coded)
	float loc[3];         // location (in mm)
	float rot[9];         // rotation matrix (column-wise)
} dtrack_meatool_type;

// Single marker data (3d):

typedef struct{
	unsigned long id;     // id number
	float quality;        // quality (0 <= qu <= 1)
	float loc[3];         // location (in mm)
} dtrack_marker_type;



// --------------------------------------------------------------------------
// DTrack class (quick&dirty-hack to enscapulate connection dependend data)

class DTrack
{
  public:

	DTrack();
	~DTrack();

	// --------------------------------------------------------------------------
	// Initialize Lib
	
	// Initialization:
	// ini (i): parameters
	// return value (o): error code

	int init(dtrack_init_type* ini);
	
	// Exit:
	// return value (o): error code

	int exit(void);


	// --------------------------------------------------------------------------
	// Receiving DTrack data

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

	int receive_udp_ascii(
		unsigned long* framenr, double* timestamp,
		int* nbodycal, int* nbody, dtrack_body_type* body, int max_nbody,
		int* nflystick, dtrack_flystick_type* flystick, int max_nflystick,
		int* nmeatool, dtrack_meatool_type* meatool, int max_nmeatool,
		int* nmarker, dtrack_marker_type* marker, int max_nmarker
	);


	// ---------------------------------------------------------------------------------------------------
	// DTrack remote control
	
	// Send remote control command (udp) to DTrack:
	
	// cmd (i): command code
	// val (i): additional value (if needed)
	// return value (o): error code
	
	int send_udp_command(unsigned short cmd, int val);

  protected:

	static int split_lines(char* str, unsigned long len, char** strarr, int maxlines);
	
	static char* get_ul(char* str, unsigned long* ul);
	static char* get_d(char* str, double* d);
	static char* get_f(char* str, float* f);
	static char* get_block(char* str, char* fmt, unsigned long* uldat, float* fdat);
	
	static int udp_init(unsigned short port);
	static int udp_exit(int sock);
	static int udp_receive(int sock, void *buffer, int maxlen, unsigned long tout_us);
	static int udp_send(int sock, void* buffer, int len, unsigned long ipaddr, unsigned short port, unsigned long tout_us);
	
	static unsigned long udp_inet_atoh(char* s);

  private:

	int _udpsock;                  // socket number for udp
	unsigned long _udptimeout_us;  // timeout for receiving and sending
	
	int _udpbufsize;               // size of udp buffer
	char* _udpbuf;                 // udp buffer
	
	unsigned long _remote_ip;      // DTrack remote command access: ip address
	unsigned short _remote_port;   // DTrack remote command access: port number
};



#endif // _ART_DTRACK_H
