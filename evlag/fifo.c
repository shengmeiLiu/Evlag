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

#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libevdev/libevdev.h>

void *fifo_create(struct fifo_header *p_f, size_t size) {

  if (!p_f) {
    fprintf(stderr, "Error! fifo_create(): fifo header is NULL");
    return NULL;
  }

  p_f->p_buf = malloc(size * sizeof(struct input_event));
  if (!p_f->p_buf) {
    fprintf(stderr, "Error! fifo_create(): unable to allocate memory.");
    return NULL;
  }
  p_f->head = 0;
  p_f->tail = 0;
  p_f->size = size;

  return p_f->p_buf;
}

void *fifo_realloc(struct fifo_header *p_f) {
  size_t new_size = p_f->size * 2;
  struct input_event *new_buf = malloc(new_size *
				       sizeof(struct input_event));
  if (new_buf == NULL) {
    fprintf(stderr, "Error! fifo_realloc(): unable to allocate memory.");
    return NULL;
  }
  
  /* Copy p_f->buf[0...p_f->head] to temp_buf. */
  memcpy(&new_buf[0], &p_f->p_buf[0], p_f->head * sizeof(struct input_event));
  
  if (p_f->tail != 0) {
    /* Number of elements to copy. */
    size_t num = p_f->size - p_f->tail;
    
    /* Copy p_f->buf[p_f->tail...p_f->size] to the end of temp_buf. */
    memcpy(&new_buf[new_size - num], &p_f->p_buf[p_f->tail],
	   sizeof(struct input_event) * num);
    
    /* Set new tail. */
    p_f->tail = new_size - num;
  }
  
  free(p_f->p_buf);
  
  p_f->p_buf = new_buf;
  p_f->size = new_size;
  
  return p_f->p_buf;
}

int fifo_pop(struct fifo_header *p_f, struct input_event *p_output) {

  if (p_f->tail != p_f->head) {

    /* Pop item off tail. */
    *p_output = p_f->p_buf[p_f->tail];
    p_f->tail++;
    if (p_f->tail == p_f->size)
      p_f->tail = 0;

  } else
    /* Fifo empty. */
    return -1; 
  
  return 0;
}

int fifo_push(struct fifo_header *p_f, const struct input_event *p_input) {

  /* Check if full. */
  if (p_f->head + 1 == p_f->tail ||
      (p_f->head + 1 == p_f->size && p_f->tail == 0)) {
    
    fprintf(stderr, "Warning! fifo_push(): Fifo buffer full (%d).\n",
	    (int) p_f->size);
    if (fifo_realloc(p_f) == NULL) {
      fprintf(stderr, "Error! fifo_push(): Failed to reallocate buffer.");
      return -1;
    }
  } 

  /* Push item on to head. */
  p_f->p_buf[p_f->head] = *p_input;
  p_f->head++;
  if (p_f->head == p_f->size)
    p_f->head = 0;
  
  return 0;
}
