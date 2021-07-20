# README

This directory contains man pages for the EvLag utilities.


## Build

Building them requires `pandoc`.

Once `pandoc` is in place, type: `make`


## Install

Gzip the `man` page(s).  e.g., `gzip /usr/local/man/man1/ms.1`

Copy the file(s) to the appropriate directory.  e.g.,
`/usr/local/man/man1`.  You will need root permission (`sudo`)
to do so.

Add the new page(s) to the `man` database: `sudo mandb`


## Read

Once installed, simply type `man FILE`.  e.g., `man evlag`.

If not installed (reading the local copy), type `man -l FILE`.


-----------------

Happy manning!

-- Mark


