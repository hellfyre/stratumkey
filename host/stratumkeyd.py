#!/usr/bin/python

import os
import sys
import daemon
import lockfile
import hashlib
import struct
import threading

import stratumkeyKeydb
import stratumkeySerial

import time

port = '/dev/ttyUSB0'
dbfile = '/var/lib/stratumkey/somedb'
random = None
outputfile = None

class SerialThread (threading.Thread):

  def __init__(self, port, dbfile):
    # Set up serial connection
    # try
    self.ser = stratumkeySerial.Serial(port)

    # Set up database
    # try

    threading.Thread.__init__(self)

  def run(self):
    self.db = stratumkeyKeydb.KeyDB(dbfile)

    while(True):
      command = self.ser.readCommand()
      if (command == 0x01): # Key auth
        cipher = hashlib.sha256()
        id = self.ser.readID()

        challenge = random.read(32)
        self.ser.writeBytes(challenge)
        response = self.ser.readBytes(32)

        key = self.db.getKey(id)

        if (key != None):
          key_and_challenge = bytearray()
          for i in range(0,32):
            a = struct.unpack('B', key[i])[0]
            b = struct.unpack('B', challenge[i])[0]
            key_and_challenge.append( struct.pack('B', (a & b)) )

          cipher.update(key_and_challenge)
          key_hash = cipher.digest()

          if (response == key_hash):
            self.ser.openDoor(outputfile)
            self.ser.flushInput()

      elif (command == 0x02): # Door bell
        relayDoorBell()

      cipher = None

def init():

  global random
  random = open('/dev/hwrng', 'rb')

  global outputfile
  outputfile = open("/var/lib/stratumkey/foobar", 'w')
  outputfile.write("One\n")

def main_loop():
  SerialThread(port, dbfile).start()

d = daemon.DaemonContext()
d.working_directory='/var/lib/stratumkey'
d.pidfile=lockfile.FileLock('/var/run/stratumkey.pid')

d.files_preserve=[outputfile]

with d:
  init()
  main_loop()
