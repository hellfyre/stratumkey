#!/usr/bin/python

import os
import sys
import daemon
import lockfile

import time

def serial_loop():

def main_loop():
  while(True):
    outputfile.write("foobar\n")
    time.sleep(5)

d = daemon.DaemonContext()
d.working_directory='/home/matthias/working/personal_projects/stratumkey/host'
d.pidfile=lockfile.FileLock('/home/matthias/working/personal_projects/stratumkey/host/stratumkey.pid')

outputfile = open("foobar", 'w')
outputfile.write("One\n")
d.files_preserve=[outputfile]

with d:
  main_loop()
