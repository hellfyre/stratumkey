#!/usr/bin/python

import sys
import os

print 'Generating random key'

if os.path.exists('dev/hwrng'):
  randomsocket = open('dev/hwrng', 'rb')
else
  randomsocket = open('/dev/random', 'rb')

outputfile = open(sys.argv[1], 'w')

for x in range(0,32):
  outputfile.write(randomsocket.read(1))
  sys.stdout.write('.')

sys.stdout.write('\n')
randomsocket.close()
outputfile.close()
