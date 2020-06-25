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

#include "evlag.h"
#include "arg_parse.h"
#include "fifo.h"
#include "threads.h"

#include <stdio.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Function prototypes. */
#ifndef fcloseall
int fcloseall(void);
#endif
void do_shut_down(int sig);

/* Condition (timer interrupt) for all threads to poll. */
pthread_cond_t rtc_interrupt = PTHREAD_COND_INITIALIZER;

int main(int argc, char **argv) {

  /* Set default values and parse arguments. */
  struct arguments args;
  
  args.verbose = VERBOSE_DEFAULT;
  args.device_count = 0;
  args.delay.tv_sec = 0;
  args.delay.tv_usec = 0;
  args.buf_size = 0;
  args.priority = PRIORITY_DEFAULT;
  args.polling_rate = POLLING_RATE_DEFAULT;
  args.logfile_name = NULL;

  if (parse_args(argc, argv, &args) < 0) {
    perror("Failed to parse arguments");
    exit(EXIT_FAILURE);
  }

  if (args.verbose) {
    printf("Verbose: on\n");
    printf("Devices: %d\n", args.device_count);
    for (int i = 0; i < args.device_count; i++)
      printf("\t%d device %s\n", i, args.event_file_name[i]);
    printf("Delay: %ld ms\n", args.delay.tv_sec*1000 +
	   args.delay.tv_usec/1000);
    printf("Buffer size: %ld\n", args.buf_size);
    printf("Priority: %d\n", args.priority);
    printf("Polling rate: %d\n", args.polling_rate);
    printf("Logfile: %s\n", args.logfile_name ? args.logfile_name : "(none)");
  }
  

  /* Set scheduler priority. */
  struct sched_param sched;
  sched.sched_priority = args.priority;
  
  if (sched_setscheduler(0, SCHED_FIFO, &sched) < 0) {
    perror("Failed to set scheduler, check your privilege");
  }

  
  /* Open device(s). */
  int fd_event[MAX_DEVICES];
  struct libevdev *event_dev[MAX_DEVICES];
  int fd_uinput[MAX_DEVICES];
  struct libevdev_uinput *uinput_dev[MAX_DEVICES];

  for (int i = 0; i < args.device_count; i++) {
    fd_event[i] = open(args.event_file_name[i], O_RDONLY);
    if (fd_event[i] < 0) {
      fprintf(stderr, "Failed to open input device '%s' : %s\n",
	      args.event_file_name[i], strerror(errno));
      exit(EXIT_FAILURE);
    }

    /* Create libevdev device and grab it. */
    if (libevdev_new_from_fd(fd_event[i], &(event_dev[i])) < 0) {
      fprintf(stderr, "Failed to init libevdev for device '%s' : %s\n",
	      args.event_file_name[i], strerror(errno));
      exit(EXIT_FAILURE);
    }
  
    if (libevdev_grab(event_dev[i], LIBEVDEV_GRAB) < 0) {
      fprintf(stderr, "Failed to init libevdev for device '%s' : %s\n",
	      args.event_file_name[i], strerror(errno));
      exit(EXIT_FAILURE);
    }


    /* Create uinput clone of device. */
    fd_uinput[i] = open("/dev/uinput", O_WRONLY);
  
    if (fd_uinput[i] < 0) {
      fprintf(stderr, "Failed to open uinput for device '%s' : %s\n",
	      args.event_file_name[i], strerror(errno));
      exit(EXIT_FAILURE);
    }
  
    if (libevdev_uinput_create_from_device(event_dev[i],
					   fd_uinput[i],
					   &(uinput_dev[i])) < 0) {
      fprintf(stderr, "Failed to create uinput for device '%s' : %s\n",
	      args.event_file_name[i], strerror(errno));
      exit(EXIT_FAILURE);
    }

  } /* End of device(s) for loop. */
  

  /* Enable RTC interrupts. */
  int fd_rtc = open("/dev/rtc", O_RDONLY);
  
  if (fd_rtc < 0) {
    perror("Failed to open RTC timer");
    exit(EXIT_FAILURE);
  }
  
  if (ioctl(fd_rtc, RTC_IRQP_SET, args.polling_rate) < 0) {
    perror("Failed to set RTC interrupts");
    exit(EXIT_FAILURE);
  }
  
  if (ioctl(fd_rtc, RTC_PIE_ON, 0) < 0) {
    perror("Failed to enable RTC interrupts");
    exit(EXIT_FAILURE);
  }

  /* Create buffer and threads. */
  int rc;
  struct fifo_header fifo[MAX_DEVICES];
  struct thread_data data[MAX_DEVICES];
  pthread_t thr[MAX_DEVICES][2];

  for (int i=0; i < args.device_count; i++) {

    /* Create fifo buffer. */
    if (fifo_create(&(fifo[i]), args.buf_size) == NULL) {
      fprintf(stderr, "Failed to allocate fifo buffer (%d) : %s\n",
	      i, strerror(errno));
      exit(EXIT_FAILURE);
    }
  
  
    /* Setup data for threads. */
    data[i].p_fifo = &(fifo[i]);
    data[i].p_args = &args;
    data[i].p_event_dev = event_dev[i];
    data[i].p_uinput_dev = uinput_dev[i];

    /* Create threads. */
    rc = pthread_create(&(thr[i][0]), NULL, read_event, &(data[i]));
    if (rc != 0) {
      fprintf(stderr, "Failed to create thread0: (%d) : %s\n",
	      rc, strerror(rc));
      exit(EXIT_FAILURE);
    }
  
    rc = pthread_create(&(thr[i][1]), NULL, write_event, &(data[i]));
    if (rc != 0) {
      fprintf(stderr, "Failed to create thread1: (%d) : %s\n",
	      rc, strerror(rc));
      exit(EXIT_FAILURE);
    }

  } /* End of create threads loop. */
  

  // Catch control-C (SIGINT) and gracefully shut down                          
  struct sigaction new_action;                                                  
  new_action.sa_handler = do_shut_down;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;
  if (sigaction(SIGINT, &new_action, NULL) == -1)                               
    fprintf(stderr, "Error catching Ctrl-C signal");
  
  /* Read from RTC device, notifiying all threads each poll. */
  do {
    rc = read(fd_rtc, NULL, sizeof(unsigned long));

    /* Notify threads. */
    int rc_broadcast = pthread_cond_broadcast(&rtc_interrupt);
    if (rc_broadcast) {
      fprintf(stderr, "Unable to broadcast to threads: %s\n",
	      strerror(rc_broadcast));
      exit(EXIT_FAILURE);
    }

  } while (rc);

  fprintf(stderr, "Failed to read RTC device: (%d) %s\n", -rc, strerror(rc));

  return EXIT_FAILURE;
}

// Called explicitly to catch ctrl-c, so exit when done.
void do_shut_down(int sig) {
  fcloseall();
  exit(sig);
}
