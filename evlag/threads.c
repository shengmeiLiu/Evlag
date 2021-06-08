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

#include "threads.h"

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <libevdev/libevdev-uinput.h>

static pthread_mutex_t fifo_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t suspend_mutex = PTHREAD_MUTEX_INITIALIZER;

void *read_event(void *p_arg) {
  struct thread_data *p_data = (struct thread_data *)p_arg;
  int rc = LIBEVDEV_READ_STATUS_SUCCESS;
  int rc_mut;
  
  do {
    struct input_event ev;
    
    rc = libevdev_next_event(p_data->p_event_dev,
			     LIBEVDEV_READ_FLAG_NORMAL |
			     LIBEVDEV_READ_FLAG_BLOCKING, &ev);
    
    /* Handle dropped SYN. */
    if (rc == LIBEVDEV_READ_STATUS_SYNC) {
      fprintf(stderr, "read_event(): Warning, syn dropped: (%d) %s\n",
	      -rc, strerror(-rc));
      
      while (rc == LIBEVDEV_READ_STATUS_SYNC)
	rc = libevdev_next_event(p_data->p_event_dev,
				 LIBEVDEV_READ_FLAG_SYNC, &ev);
    }
    
    /* Add delay to input. */
    timeradd(&ev.time, &p_data->p_args->delay, &ev.time);
    
    rc_mut = pthread_mutex_lock(&fifo_mutex);
    if (rc_mut != 0)
      fprintf(stderr, "read_event(): Failed to lock mutex: (%d) %s\n",
	      rc, strerror(rc_mut));
    
    if (fifo_push(p_data->p_fifo, &ev) < 0)
      fprintf(stderr, "read_event(): Failed to fifo_push().");
    
    rc_mut = pthread_mutex_unlock(&fifo_mutex);
    if (rc_mut != 0)
      fprintf(stderr, "read_event(): Failed to unlock mutex: (%d) %s\n",
	      rc, strerror(rc_mut));
    
  } while (rc == LIBEVDEV_READ_STATUS_SYNC ||
	   rc == LIBEVDEV_READ_STATUS_SUCCESS);
  
  
  if (-rc == ENODEV)
    fprintf(stderr, "read_event(): Device disconnected: (%d) %s\n",
	    -rc, strerror(-rc));
  else
    fprintf(stderr, "read_event(): Failed to read input device: (%d) %s\n",
	    -rc, strerror(-rc));
  
  exit(EXIT_FAILURE);
}

void *write_event(void *p_arg) {
  extern pthread_cond_t rtc_interrupt; // Declared in main().
  struct thread_data *p_data = (struct thread_data *)p_arg;
  struct timeval current_time;
  struct input_event ev;
  int have_event = 0;
  int rc_fifo = 0;    
  int rc = 0;
  int start = 0;
  FILE *fp = NULL;

  /* Prepare logfile. */
  if (p_data->p_args->logfile_name) {

    if (gettimeofday(&current_time, NULL) == 0)
      start = current_time.tv_sec * 1000 + current_time.tv_usec / 1000;

    /* Get device name. */
    char *ev_name = strrchr(libevdev_get_name(p_data->p_event_dev), ' ') + 1;

    int len = strlen(p_data->p_args->logfile_name) + strlen(ev_name) + 6;
    char *file_name = (char *) malloc(len);
    if (!file_name) {
      fprintf(stderr, "write_event(): Unable to malloc().  No logging.");
      fp = NULL;
    } else {
      sprintf(file_name, "%s-%s.log", p_data->p_args->logfile_name, ev_name);
      fp = fopen(file_name, "w");
      if (!fp)
	fprintf(stderr, "write_event(): Unable to open file: %s. No logging.",
		file_name);
      free(file_name);
    }

    if (fp)
      fprintf(fp, "millisec, event-type, event-code, event-value\n");

  } // Opened logfile.

  while (1) {

    rc = gettimeofday(&current_time, NULL);
    if (rc != 0)
      fprintf(stderr, "write_event(): Failed to gettimeofday(): %s\n",
	      strerror(errno));

    /* Compare current time to time of latest event. */
    if (timercmp(&current_time, &ev.time, >=)) {

      do {

	if (have_event) {
	  rc = libevdev_uinput_write_event(p_data->p_uinput_dev,
					   ev.type, ev.code, ev.value);
	  if (rc != 0)
	    fprintf(stderr, "write_event(): Failed writing uinput event: %s\n",
		    strerror(-rc));
	  
	  /* Write event to logfile. */
	  if (fp) {

	    int now = ev.time.tv_sec * 1000 + ev.time.tv_usec / 1000;
	    fprintf(fp, "%d, %d, %d, %d\n",
		    now - start,
		    ev.type,
		    ev.code,
		    ev.value);
	  }

	  have_event = 0;
	}

	rc = pthread_mutex_lock(&fifo_mutex);
	if (rc != 0)
	  fprintf(stderr, "write_event(): Failed to lock mutex: %s\n",
		  strerror(rc));
      
	rc_fifo = fifo_pop(p_data->p_fifo, &ev);
	if (rc_fifo == 0)
	  have_event = 1;
	else
	  have_event = 0;

	rc = pthread_mutex_unlock(&fifo_mutex);
	if (rc != 0) 
	  fprintf(stderr, "write_event(): Failed to unlock mutex: %s\n",
		  strerror(rc));

      } while (timercmp(&current_time, &ev.time, >=) && have_event);

    }

    /* Block until next time interval interrupt. */
    rc = pthread_mutex_lock(&suspend_mutex);
    if (rc != 0) 
      fprintf(stderr, "write_event(): Failed to lock mutex: %s\n",
	      strerror(rc));

    rc = pthread_cond_wait(&rtc_interrupt, &suspend_mutex);
    if (rc != 0)
      fprintf(stderr, "write_event(): Failed to wait on condition: %s\n",
	      strerror(rc));

    rc = pthread_mutex_unlock(&suspend_mutex);
    if (rc != 0) 
      fprintf(stderr, "write_event(): Failed to unlock mutex: %s\n",
	      strerror(rc));

  } /* End of while (1) */

}
