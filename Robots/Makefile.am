SUBDIRS = empty rotate_and_fire seek_and_destroy


# Creates hard links to robots in subdirectories

link_robots: 
	find . -path "./*/*.robot" -type f -links 1 -exec ln -v \{\} . \;


# If your robot already has some hard links, this may work better:

#link_robots: 
#	find . -path "./*/*.robot" -type f --exec ln -vf \{\} . \;


