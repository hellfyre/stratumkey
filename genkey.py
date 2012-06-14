#!/usr/bin/python

import sys

print 'Generating random key'

randomsocket = open('/dev/random', 'r')
outputfile = open(sys.argv[1], 'w')

for x in range(0,32):
  outputfile.write(randomsocket.read(1))
  sys.stdout.write('.')

sys.stdout.write('\n')
randomsocket.close()
outputfile.close()
