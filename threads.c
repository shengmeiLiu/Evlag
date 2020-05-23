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

#include "threads.h"

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <libevdev/libevdev-uinput.h>

static pthread_mutex_t fifo_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t suspend_mutex = PTHREAD_MUTEX_INITIALIZER;

void *get_event(void *arg) {
  struct thread_data *data = (struct thread_data *)arg;
  int rc = LIBEVDEV_READ_STATUS_SUCCESS;
  int rc_mut;
  
  do {
    struct input_event ev;
    
    rc = libevdev_next_event(data->event_dev,
			     LIBEVDEV_READ_FLAG_NORMAL |
			     LIBEVDEV_READ_FLAG_BLOCKING, &ev);
    
    /* Handle dropped SYN. */
    if (rc == LIBEVDEV_READ_STATUS_SYNC) {
      fprintf(stderr, "Warning, syn dropped: (%d) %s\n", -rc, strerror(-rc));
      
      while (rc == LIBEVDEV_READ_STATUS_SYNC)
	rc = libevdev_next_event(data->event_dev,
				 LIBEVDEV_READ_FLAG_SYNC, &ev);
    }
    
    /* Add delay to input. */
    timeradd(&ev.time, &data->args->delay, &ev.time);
    
    rc_mut = pthread_mutex_lock(&fifo_mutex);
    if (rc_mut != 0)
      fprintf(stderr, "Failed to lock mutex: (%d) %s\n", rc, strerror(rc_mut));
    
    if (fifo_push(data->fifo, &ev) < 0) {
      handle_full_fifo(data->fifo, &data->args->resize_factor);
      fifo_push(data->fifo, &ev);
    }
    
    rc_mut = pthread_mutex_unlock(&fifo_mutex);
    if (rc_mut != 0)
      fprintf(stderr, "Failed to unlock mutex: (%d) %s\n", rc, strerror(rc_mut));
    
  } while (rc == LIBEVDEV_READ_STATUS_SYNC ||
	   rc == LIBEVDEV_READ_STATUS_SUCCESS);
  
  
  if (-rc == ENODEV)
    fprintf(stderr, "Device disconnected: (%d) %s\n", -rc, strerror(-rc));
  else
    fprintf(stderr, "Failed to read input device: (%d) %s\n", -rc, strerror(-rc));
  
  exit(EXIT_FAILURE);
}

void *write_event(void *arg) {
  extern pthread_cond_t rtc_interrupt; // Declared in main().
  struct thread_data *data = (struct thread_data *)arg;
  struct timeval current_time;
  struct input_event ev;
  int have_event = 0; // First event is a special case.
  int rc_fifo = 0;    
  int rc;

  while (1) {

    rc = gettimeofday(&current_time, NULL);
    if (rc != 0)
      fprintf(stderr, "Failed to gettimeofday() : %s\n", strerror(errno));
      
    if (timercmp(&current_time, &ev.time, >=))  {

      do {

	if (have_event) {
	  rc = libevdev_uinput_write_event(data->uinput_dev, ev.type,
					   ev.code, ev.value);
	  if (rc != 0)
	    fprintf(stderr, "Failed to write uinput event: %s\n",  strerror(-rc));
	}

	rc = pthread_mutex_lock(&fifo_mutex);
	if (rc != 0)
	  fprintf(stderr, "Failed to lock mutex: %s\n", strerror(rc));
      
	rc_fifo = fifo_pop(data->fifo, &ev);
	if (rc_fifo == 0)
	  have_event = 1;

	rc = pthread_mutex_unlock(&fifo_mutex);
	if (rc != 0) 
	  fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(rc));

      } while (timercmp(&current_time, &ev.time, >=) && rc_fifo == 0);

    }

    /* Block until next time interval interrupt. */
    rc = pthread_mutex_lock(&suspend_mutex);
    if (rc != 0) 
      fprintf(stderr, "Failed to lock mutex: %s\n", strerror(rc));

    rc = pthread_cond_wait(&rtc_interrupt, &suspend_mutex);
    if (rc != 0)
      fprintf(stderr, "Failed to wait on condition: %s\n", strerror(rc));

    rc = pthread_mutex_unlock(&suspend_mutex);
    if (rc != 0) 
      fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(rc));

  } // End of while (1)

}

/*
void *write_event(void *arg) {
  extern pthread_cond_t rtc_interrupt; // Declared in main().

  struct thread_data *data = (struct thread_data *)arg;
  int rc;
  
  while (1) {

    struct input_event ev;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    struct timeval temp = ev.time;
    
    if (timercmp(&current_time, &ev.time, >=)) {
      int rc_fifo = 0;
      
      do {
	rc = libevdev_uinput_write_event(
					 data->uinput_dev, ev.type,
					 ev.code, ev.value);
	
	if (rc != 0)
	  fprintf(stderr, "Failed to write uinput event: %s\n",  strerror(-rc));
	
	rc = pthread_mutex_lock(&fifo_mutex);
	if (rc != 0)
	  fprintf(stderr, "Failed to lock mutex: %s\n", strerror(rc));
	
	rc_fifo = fifo_pop(data->fifo, &ev);
	
	rc = pthread_mutex_unlock(&fifo_mutex);
	if (rc != 0) 
	  fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(rc));
	
      } while (timercmp(&temp, &ev.time, ==) && rc_fifo == 0);
    }

    // Block until next time interval interrupt. 
    pthread_mutex_lock(&suspend_mutex);
    pthread_cond_wait(&rtc_interrupt, &suspend_mutex);
    pthread_mutex_unlock(&suspend_mutex);

  } // End of while (1)

}
*/
