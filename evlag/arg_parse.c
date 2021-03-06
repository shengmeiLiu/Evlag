/*
 * evlag
 *
 * Copyright 2021 Mark Claypool, WPI
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

#include "arg_parse.h"

#include <stdlib.h>
#include <sys/time.h>
#include <argp.h>
#include <libevdev/libevdev.h>

const char *argp_program_version = "evlag 2.5";

const char *argp_program_bug_address =
  "Mark Claypool <claypool@cs.wpi.edu>";

static char doc[] =
  "\nevlag -- A simple tool for lagging and monitoring input.\n"
  "Must be run as superuser.";

static char args_doc[] =
  "--device <FILE> [--device <FILE> ... (max 10)] [--lag <NUM>]";

static struct argp_option options[] = {
  {"device", 'd', "FILE", 0, "Input device /dev/input/eventX to lag"},
  {"lag", 'l', "NUM", 0, "Length of delay (ms)"},
  {"buffer", 'b', "NUM", 0, "Size of buffer (in events)"},
  {"Hz", 'h', "NUM", 0, "Polling rate of real time clock (1 - 8192, default 2048)"},
  {"priority", 'p', "NUM", 0, "Scheduler priority (1 - 99, default 20)"},
  {"file", 'f', "FILE", 0, "Logfile for events (default none)"},
  {"verbose", 'v', 0, 0, "Verbose on (default off)"},
  {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *args = state->input;

  switch (key) {

  case 'd':
    if (args->device_count == MAX_DEVICES) {
      argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
    }
    args->event_file_name[args->device_count] = arg;
    args->device_count += 1;
    break;

  case 'f':
    args->logfile_name = arg;
    break;
    
  case 'l':;
    unsigned long long m_sec = strtoull(arg, NULL, 10);
    time_t sec = m_sec / 1000;
    args->delay.tv_sec = sec;
    args->delay.tv_usec = (m_sec - sec * 1000) * 1000;
    break;

  case 'b':
    args->buf_size = strtoull(arg, NULL, 10);
    break;

  case 'v':
    args->verbose = 1;
    break;
    
  case 'h':
    args->polling_rate = strtol(arg, NULL, 10);
    break;

  case 'p':
    args->priority = strtol(arg, NULL, 10);
    break;

  case ARGP_KEY_END:
    /* Check if file is specified. */
    if (args->device_count == 0)
      argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
    
    /*
     * If buffer size is not specified, calculate buffer size by:
     * [assumed number of events per ms] * [time of delay in ms]
     */
    if (args->buf_size == 0) {
      args->buf_size += 40 * args->delay.tv_sec * 1000;
      args->buf_size += 40 * args->delay.tv_usec / 1000;
      if (args->buf_size == 0)
	args->buf_size = 40;
    }
    break;
  }
  
  return 0;
}

error_t parse_args(int argc, char **argv, struct arguments *args) {
  struct argp argp = {options, parse_opt, args_doc, doc};
  
  return argp_parse(&argp, argc, argv, 0, 0, args);
}
