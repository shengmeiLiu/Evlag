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

#include <stdlib.h>
#include <libevdev/libevdev.h>

struct fifo_header {
  struct input_event *p_buf;
  size_t head; // Next item to add.
  size_t tail; // Next item to remove.
  size_t size; // Number of items.
};

/*
 * Allocate memory according to size (in number of input events).
 * Return pointer to allocated buffer, NULL if error.
 */
void *fifo_create(struct fifo_header *f, size_t size);

/*
 * Reallocate buffer to be twice the size.
 * Return a pointer to new buffer, NULL if error.
 */
void *fifo_realloc(struct fifo_header *p_f);

/*
 * Write element from fifo tail to *p_output.
 * If fifo is empty, return -1, else return 0.
 */
int fifo_pop(struct fifo_header *p_f, struct input_event *p_output);

/*
 * Writes element from *p_input to fifo head.
 * If fifo is full, return -1, else return 0.
 */
int fifo_push(struct fifo_header *f, const struct input_event *p_input);
