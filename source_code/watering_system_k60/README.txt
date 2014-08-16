Automatic Watering System Source Code
=====================================

Key features:

1. Unattended mode of operation
2. Support for watering cycles and dry periods
3. Watering via web interface
4. Flexible configuration
5. TCP/IP stack with DHCP
6. Web interface

Description:

This is code uses Freescale MQX 4.1 and runs on Freescale processor K60.
Code is compiled with CodeWarrior 10.6.

Application runs on 4 tasks:
1. Web server
2. Network
3. Watering System
4. Pump control

Web pages are stored on trivial file system in flash (tfs:).
mktfs.exe generates tfs_data.c for web_pages folder contents.

NOTE 1:
There is some sort of a bug in mktfs.exe and some files are corrupted.
To avoid that I split bootstrap.css into 5 files and added some extra dummy file:
bootstrap2.5.css which is used only to keep this corrupted segments out of the way.

NOTE 2:
Server Side Includes (ASP like) have limitation of name to just a few characters.
Hence very short (hard to read) callback function names: <%p_left%>

