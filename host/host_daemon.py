#!/usr/bin/python

import os
import sys
import daemon
import lockfile
import hashlib
from threading import Thread

import stratumkeyKeydb
import stratumkeySerial

import time

def init():
  port = '/dev/ttyUSB0'
  dbfile = 'somedb'

  cipher = hashlib.sha256()

  # Set up serial connection
  # try
  ser = stratumkeySerial.Serial(port)

  # Set up database
  # try
  db = stratumkeyKeydb.KeyDB(dbfile)

def serial_loop():
  while(True):
    command = ser.readBytes(1)
    if (command == '\x01'): # Key auth
      id = ser.readID()

      challenge = random.read(32)
      ser.writeBytes(challenge)
      response = ser.readBytes(32)

      key = db.getKey(id)

      if (key != None):
        key_and_challenge = bytearray()
        for i in range(0,32):
          a = struct.unpack('B', key[i])[0]
          b = struct.unpack('B', challenge[i])[0]
          key_and_challenge.append( struct.pack('B', (a & b)) )

        cipher.update(key_and_challenge)
        key_hash = cipher.digest()

        if (response == key_hash):
          ser.openDoor()

    elif (command == '\x02'): # Door bell
      relayDoorBell()

def main_loop():
  serialThread = Thread(target=serial_loop)
  serialThread.start()

  while(True):
    time.sleep(5)

d = daemon.DaemonContext()
d.working_directory='/home/matthias/working/personal_projects/stratumkey/host'
d.pidfile=lockfile.FileLock('/home/matthias/working/personal_projects/stratumkey/host/stratumkey.pid')

init()
#outputfile = open("foobar", 'w')
#outputfile.write("One\n")
#d.files_preserve=[outputfile]

with d:
  main_loop()
