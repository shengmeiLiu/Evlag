# README

Based on:
<https://linuxconfig.org/easy-way-to-create-a-debian-package-and-local-package-repository>

## Create

1. Create/edit DEBIAN/control
 
    - Make sure permissions on file and directory are u=rwx,go=rx 
	
2. Build package

    - `dpkg-deb --build . evlag.deb`

3. Rename based on version

    - Example: `mv evlag.deb evlag-2.5_amd64.deb`
	
4. Make packages

    - `dpkg-scanpackages . | gzip -c9  > Packages.gz`

    Note!  This will need to be run in the directory
	if more than one.

## Install

A. Add source

    - `echo "deb [trusted=yes] http://www.cs.wpi.edu/~claypool/packages ./" | sudo tee -a /etc/apt/sources.list`
	
B. Synchronize repositories:

    - `sudo apt-get update`
	
C. Install

    - `sudo apt get install evlag`

---------------

Happy packaging!

-- Mark
