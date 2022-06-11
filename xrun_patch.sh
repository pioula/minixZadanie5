patch -t -p1 < pu429640.patch
cd /usr/src; make includes
cd /usr/src/minix/servers/vfs/; make && make install