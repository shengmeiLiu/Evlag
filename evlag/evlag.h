/*
 * evlag
 *
 * Copyright 2020 Mark Claypool, WPI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <stddef.h>
#include <sys/time.h>

/* Constants. */
#define MAX_DEVICES              10
#define VERBOSE_DEFAULT           0
#define PRIORITY_DEFAULT         20
#define POLLING_RATE_DEFAULT   2048

/* Values provided from command line. */
struct arguments {
  int verbose;                 // True for more messages.
  int device_count;            // Number of devices being lagged.
  char *event_file_name[MAX_DEVICES]; // Array of device file names.
  struct timeval delay;	       // Delay to add to each device.
  size_t buf_size;             // Input buffer size (MB).
  int priority;                // Priority for scheduler.
  unsigned short polling_rate; // Polling rate (same for all devices).
  char *logfile_name;          // Logfile base name.
};

