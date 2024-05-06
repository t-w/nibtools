/*
 * NIBTOOLS
 * Copyright (c) Pete Rittwage <peter(at)rittwage(dot)com>
 * based on MNIB by Markus Brenner <markus(at)brenner(dot)de>
 */


#include "nibtools_version.h"

#define FL_STEPTO      			0x00
#define FL_MOTOR       		0x01
#define FL_RESET       			0x02
#define FL_READWOSYNC  0x03
#define FL_READNORMAL  0x04
#define FL_READIHS 			0x05
#define FL_DENSITY     		0x06
#define FL_SCANKILLER  	0x07
#define FL_SCANDENSITY 	0x08
#define FL_READMOTOR   	0x09
#define FL_TEST        			0x0a
#define FL_WRITE   				0x0b
#define FL_CAPACITY    		0x0c
#define FL_ALIGNDISK 		0x0d
#define FL_VERIFY_CODE 	0x0e
#define FL_FILLTRACK 		0x0f
#define FL_READMARKER  0x10

#define DISK_NORMAL    0

#define IMAGE_NIB      	0	/* destination image format */
#define IMAGE_D64      	1
#define IMAGE_G64      	2
#define IMAGE_NB2			3

#define BM_MATCH       	0x10 /* not used but exists in very old images */
#define BM_NO_CYCLE 	0x20
#define BM_NO_SYNC		0x40
#define BM_FF_TRACK		0x80

#define DENSITY_SAMPLES 2

/* custom density maps for reading */
#define DENSITY_STANDARD 0
#define DENSITY_RAPIDLOK	1

#define MODE_READ_DISK     	0
#define MODE_WRITE_DISK    	1
#define MODE_UNFORMAT_DISK 	2
#define MODE_WRITE_RAW	   	3
#define MODE_TEST_ALIGNMENT 4
#define MODE_SPEED_ADJUST 5

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)


#ifndef DJGPP
#include <opencbm.h>
#endif

/* global variables */
extern char bitrate_range[4];
extern char bitrate_value[4];
extern char density_branch[4];
extern char *cbm_adapter;
extern BYTE density_map;
extern BYTE skew_map[];
extern BYTE fillbyte;
extern BYTE copyprot;
extern BYTE drive;
extern FILE * fplog;
extern float motor_speed;
extern size_t error_retries;
extern int skew;
extern int mode;
extern int read_killer;
extern int align_disk;
extern int force_density;
extern int track_match;
extern int interactive_mode;
extern int gap_match_length;
extern int cap_min_ignore;
extern int verbose;
extern int ihs;
extern int start_track, end_track, track_inc;
extern int fix_gcr, reduce_sync, increase_sync;
extern int imagetype, auto_capacity_adjust;
extern int extended_parallel_test;
extern int force_nosync;
extern int rawmode;
extern int rpm_real;
extern int unformat_passes;
extern int align_delay;
extern int presync;
extern int use_floppycode_srq;
extern int override_srq;
extern int extra_capacity_margin;
extern int sync_align_buffer;
extern int fattrack;
extern int old_g64;

#include "ihs.h"

/* common */
void usage(void);

/* nibread.c */
int disk2file(CBM_FILE fd, char * filename);
void parallel_test(int interations);

/* nibwrite.c */
int loadimage(char * filename);
int writeimage(CBM_FILE fd);

/* fileio.c */
#include "fileio.h"

/* read.c */
BYTE read_halftrack(CBM_FILE fd, int halftrack, BYTE * buffer);
BYTE paranoia_read_halftrack(CBM_FILE fd, int halftrack, BYTE * buffer);
int read_floppy(CBM_FILE fd, BYTE *track_buffer, BYTE *track_density, size_t *track_length);
int write_nb2(CBM_FILE fd, char * filename);
void get_disk_id(CBM_FILE fd);
BYTE scan_density(CBM_FILE fd);
int TrackAlignmentReport(CBM_FILE fd);

/* write.c */
void master_disk(CBM_FILE fd, BYTE *track_buffer, BYTE *track_density, size_t *track_length);
void master_disk_raw(CBM_FILE fd, BYTE *track_buffer, BYTE *track_density, size_t *track_length);
void prep_track(CBM_FILE fd, BYTE *track_buffer, BYTE *track_density, int track, size_t tracklen);
void write_raw(CBM_FILE fd, BYTE *track_buffer, BYTE *track_density, size_t *track_length);
void unformat_disk(CBM_FILE fd);
void zero_track(CBM_FILE fd, int track);
unsigned int track_capacity(CBM_FILE fd);
void init_aligned_disk(CBM_FILE fd);
void adjust_target(CBM_FILE fd);
void speed_adjust(CBM_FILE fd);
void kill_track(CBM_FILE fd, int track);

/* drive.c  */
#include "drive.h"
