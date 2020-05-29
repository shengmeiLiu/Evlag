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

#include "arg_parse.h"
#include "fifo.h"

#include <libevdev/libevdev-uinput.h>

/*
 * Structure is passed to read_event
 * and write_event functions.
 */
struct thread_data {
  struct arguments *p_args;
  struct fifo_header *p_fifo;
  struct libevdev *p_event_dev;
  struct libevdev_uinput *p_uinput_dev;
};

/*
 * Read events, adding delay, and writing to fifo buffer.
 */
void *read_event(void *p_arg);

/*
 * Pull events from fifo buffer, comparing event time with current
 * time, and write events to uinput device.
 */
void *write_event(void *p_arg);
