# README

v1.1

Instructions to create a local Debian package and repository.

Based on:

<https://linuxconfig.org/easy-way-to-create-a-debian-package-and-local-package-repository>

Commands are examples only and may need to be modified to fit your installation.

## Create Package

1a. Create/edit control file.

    - `mkdir DEBIAN/control`

    - `chmod u=rwx,go=rx DEBIAN/control DEBIAN`
	
    - Inside the control file, enter:

````
  Package: evlag
  Version: 2.5
  Section: custom
  Priority: optional
  Architecture: amd64
  Essential: no
  Installed-Size: 23840
  Maintainer: Mark Claypool (claypool@cs.wpi.edu)
  Description: EvLag is a simple tool for lagging and monitoring device input in Linux.

````

1b. Create/edit postinst file.

````
  # Change permissions and ownership.
  chown -R root /usr/local/evlag
  chgrp -R root /usr/local/evlag
  chmod -R og+rx /usr/local/evlag

  # Display message for where things are installed.
  echo "EvLag utilities installed in /usr/local/evlag/:"
  ls -1d /usr/local/evlag/*

````

1c. Create directory structure.

    - `mkdir -p usr/local/evlag`
    - `mkdir -p usr/local/bin/`

1d. Copy in binaries/utilities and make links.

    - `cp ../evlag/evlag usr/local/evlag`
    - `cp ../evdetect/evdetect.sh usr/local/evlag`
    - `cp ../evparse/evparse.py usr/local/evlag`
    - `ln -s  usr/local/bin/evlag /usr/local/bin/evlag`
    - `ln -s  usr/local/bin/evdetect.sh /usr/local/bin/evdetect.sh`
    - `ln -s  usr/local/bin/evparse.py /usr/local/bin/evparse.py`

2. Build package file.

    - `dpkg-deb --build . evlag.deb`

3. Rename package based on version and architecture.

    - `mv evlag.deb evlag-2.5_amd64.deb`
	

## Put on Web server

1. Setup/use a Web server with permissions to add a directory and files.
For setup, there are a lot of guides on the Internet for this.

2. Create a directory to host Debian packages. 

    - `mkdir /home/claypool/public_html/packages`

3. Copy the file in step #3 above into the new directory.

    - `cp evlag-2.5_amd64.deb /home/claypool/public_html/packages`

4. Make the packages index.

    - `cd /home/claypool/public_html/packages`
    - `dpkg-scanpackages . | gzip -c9  > Packages.gz`

5. Set permissions.

    - `chmod og+rx /home/claypool/public_html/packages`
	- `chmod og+r /home/claypool/public_html/packages/*`


## Install Package

A. Add source Web server and directory.

    - `echo "deb [trusted=yes] http://www.cs.wpi.edu/~claypool/packages ./" | sudo tee -a /etc/apt/sources.list`

B. Synchronize repositories.

    - `sudo apt update`
	
C. Install

    - `sudo apt install evlag`
	
D. Uninstall (if needed)

    - `sudo apt remove evlag`

---------------

Happy packaging!

-- Mark
