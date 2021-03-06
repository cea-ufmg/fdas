/**
 * Device module for Crossbow's AHRS400 Attitude and Heading Reference System.
 */


#include <argp.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ahrs400.h"


/** Mavlink system identifier */
#define MAVLINK_SYSID 1

/** Mavlink compenent identifier, equal to MAV_COMP_ID_IMU */
#define MAVLINK_COMPID 200

/** Program version. */
const char *argp_program_version = "ahrs400-read 0.1";

/** Bug report address. */
const char *argp_program_bug_address = "https://github.com/cea-ufmg/fdas3";

/** Program documentation. */
static char doc[] = "ahrs400-read -- Read from a Crossbow AHRS400.";

/** Description of the accepted arguments. */
static char args_doc[] = "AHRS_PORT";

/** Program options structure. */
static struct argp_option options[] = {
    {"logtxt", 't', "FILE", 0, "Write received data as text to FILE"},
    {"logbin", 'b', "FILE", 0, "Write binary MAVLink stream FILE"},
    {"verbose", 'v', 0, 0, "Write received data as text to STDOUT"},
    {"udp", 'u', "HOST", OPTION_ARG_OPTIONAL,
     "Send MAVLink messages via UDP to HOST, defaults to 224.0.0.1"},
    {"udp-port", 'p', "UDPPORT", 0,
     "UDP port to send MAVLink messages to, defaults to 38400, implies --udp"},
    {0}
};

/** Program arguments structure. */
typedef struct arguments {
    char *ahrs_port;
    char *text_log;
    char *binary_log;
    bool verbose;
    bool use_udp;
    char *udp_host;
    uint16_t udp_port;
} arguments_t;

/** Program output streams structure */
typedef struct output_streams {
    int udp_sock;
    FILE *binary_log;
    FILE *text_log;
} output_streams_t;


/** Argument parser function */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    //Get the arguments structure to write the parsed options
    arguments_t *arguments = state->input;
    
    switch (key) {
    case 't':
        arguments->text_log = arg;
        break;

    case 'v':
        arguments->verbose = true;
        break;
        
    case 'b':
        arguments->binary_log = arg;
        break;

    case 'u':
        arguments->use_udp = true;
        if (arg)
            arguments->udp_host = arg;
        break;
        
    case 'p':
	arguments->use_udp = true;
	{
	    char *endptr = 0;
	    unsigned long udp_port = strtoul(arg, &endptr, 0);
	    if (*endptr) {
                argp_error(state, "UPDPORT argument must be an integer.");
	    }
            if (udp_port > 65535) {
                argp_error(state, "UPDPORT number too large.");
            }
	    arguments->udp_port = udp_port;
	}
        break;
	        
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1)
          argp_error(state, "Too many arguments.");
      arguments->ahrs_port = arg;
      break;

    case ARGP_KEY_END:
        if (state->arg_num < 1)
            argp_error(state, "Not enough arguments.");
      break;
        
    default:
        return ARGP_ERR_UNKNOWN;        
    }
    
    return 0;
}


/** Argument parser object. */
static struct argp argp = {options, parse_opt, args_doc, doc};


/**
 * Open the program output streams
 */
void open_output_streams(arguments_t *args, output_streams_t *out) {
    // Open text log
    if (args->text_log) {
	out->text_log = fopen(args->text_log, "w");
	if (!out->text_log) {
	    syslog(LOG_ERR, "Error opening text log: %s", strerror(errno));
	    exit(EXIT_FAILURE);
	}
        // Print file header
        int status = fprintf(
            out->text_log, "%% time[us]\txacc[m/s^2]\tyacc\tzacc\t"
            "xgyro[rad/s]\tygyro\tzgyro\txmag[gauss]\tymag\tzmag\t"
            "xmag[gauss]\tymag\tzmag\troll[rad]\tpitch\tyaw\t"
            "temperature[C]\tsensor_time\n"
        );
        if (status < 0)
            syslog(LOG_ERR, "Error writing to text log: %s", strerror(errno));
    }
    
    // Open binary log
    if (args->binary_log) {
	out->binary_log = fopen(args->binary_log, "w");
	if (!out->binary_log) {
	    syslog(LOG_ERR, "Error opening binary log: %s", strerror(errno));
	    exit(EXIT_FAILURE);
	}
    }
    
    // Open UDP socket
    if (args->use_udp) {
        struct sockaddr_in host_addr;
        socklen_t addr_len = sizeof(host_addr);
        struct hostent *hostent = gethostbyname(args->udp_host);
        if (!hostent) {
            syslog(LOG_ERR, "Could not find host address `%s`", args->udp_host);
	    exit(EXIT_FAILURE);
        }
        if (hostent->h_addrtype != AF_INET || hostent->h_length != 4) {
            syslog(LOG_ERR, "Only IPv4 hosts supported.");
	    exit(EXIT_FAILURE);
        }
        memset((void *)&host_addr, 0, addr_len);
        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(args->udp_port);
        memcpy((void *)&host_addr.sin_addr,
               hostent->h_addr_list[0], hostent->h_length);
        
        out->udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (out->udp_sock < 0) {
	    syslog(LOG_ERR, "Error creating UDP socket: %s", strerror(errno));
	    exit(EXIT_FAILURE);
	}

        if (connect(out->udp_sock, (struct sockaddr *)&host_addr, addr_len)) {
	    syslog(LOG_ERR, "Error connecting socket: %s", strerror(errno));
	    exit(EXIT_FAILURE);
        }
    }
}


void log_text(const mavlink_ahrs400_angle_t *angle, FILE *out) {
    if (out) {
        int status = fprintf(
            out, "%llu\t%e\t%e\t%e\t%e\t%e\t%e\t%e\t%e\t%e\t"
            "%e\t%e\t%e\t%e\t%u\n", (unsigned long long) angle->time_usec,
            angle->xacc, angle->yacc, angle->zacc,
            angle->xgyro, angle->ygyro, angle->zgyro,
            angle->xmag, angle->ymag, angle->zmag,
            angle->roll, angle->pitch, angle->yaw,
            angle->temperature, angle->sensor_time
        );
        if (status < 0)
	    syslog(LOG_ERR, "Error writing to text log: %s", strerror(errno));
    }    
}


void output_mavlink_msg(mavlink_message_t *msg, output_streams_t *out) {
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    size_t len = mavlink_msg_to_send_buffer(buf, msg);
    
    // Output to binary log
    if (out->binary_log)
        if (!fwrite(buf, len, 1, out->binary_log))
	    syslog(LOG_ERR, "Error writing to binary log: %s", strerror(errno));
    
    // Output to UDP socket
    if (out->udp_sock > 0)
        if (send(out->udp_sock, buf, len, 0) != len)
	    syslog(LOG_ERR, "Error sending UDP message: %s", strerror(errno));
}


void output_angle_raw(const mavlink_ahrs400_angle_raw_t *angle_raw,
                      output_streams_t *out) {
    mavlink_message_t msg;
    mavlink_msg_ahrs400_angle_raw_encode(
        MAVLINK_SYSID, MAVLINK_COMPID, &msg, angle_raw
    );
    output_mavlink_msg(&msg, out);
}


void output_angle(const mavlink_ahrs400_angle_t *angle, output_streams_t *out) {
    mavlink_message_t msg;
    mavlink_msg_ahrs400_angle_encode(
        MAVLINK_SYSID, MAVLINK_COMPID, &msg, angle
    );
    output_mavlink_msg(&msg, out);
}


int main(int argc, char **argv) {
    // Parse command line arguments
    arguments_t arguments = {.udp_host="224.0.0.1", .udp_port=38400};
    output_streams_t output_streams = {.udp_sock=-1};
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // Setup syslog
    openlog(0, LOG_PERROR, 0);

    // Open the output streams
    open_output_streams(&arguments, &output_streams);
    
    // Open AHRS port
    FILE *ahrs_stream = ahrs_open(arguments.ahrs_port);
    if (!ahrs_stream)
        return EXIT_FAILURE;

    // Put AHRS into polled mode for configuration
    if (ahrs_set_polled(ahrs_stream))
        return EXIT_FAILURE;
        
    // Wait for pending data to arrive and clear buffers
    fflush(ahrs_stream);
    sleep(1);
    ahrs_purge(ahrs_stream);
    
    // Ping the AHRS
    if (ahrs_ping(ahrs_stream))
        return EXIT_FAILURE;
    
    // Set the mode
    if (ahrs_set_mode(ahrs_stream, AHRS_ANGLE_MODE)
        || ahrs_set_continuous(ahrs_stream))
        return EXIT_FAILURE;

    // Read loop
    for (;;) {
        mavlink_ahrs400_angle_raw_t angle_raw;
        if (ahrs_get_angle_raw(ahrs_stream, &angle_raw))
            return EXIT_FAILURE;

        mavlink_ahrs400_angle_t angle;
        ahrs_angle_conv(&angle_raw, &angle);

        output_angle_raw(&angle_raw, &output_streams);
        output_angle(&angle, &output_streams);
        
        log_text(&angle, output_streams.text_log);
        if (arguments.verbose)
            log_text(&angle, stdout);
    }
    
    return 0;
}
