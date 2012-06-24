import serial
import struct
import sys
import time
import hashlib

class Serial:
  def __init__(self, port):
    self.ser = serial.Serial(port, 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE, None, 0, 0, None)
    self.ser.open()
    self.ser.flushInput()
    self.cipher = hashlib.sha256()

  def __del__(self):
    self.ser.close()

  def readBytes(self, count):
    arr = bytearray()
    for i in range(0,count):
      arr.append(struct.unpack('B', self.ser.read(1))[0])
    return arr

  def writeBytes(self, data):
    return self.ser.write(buffer(data))

  def readCommand(self):
    cmd = struct.unpack('B', self.ser.read(1))[0]
    return cmd

  def readID(self):
    d1=struct.unpack('B', self.ser.read(1))[0]
    d2=struct.unpack('B', self.ser.read(1))[0]
    return (d1<<8) + d2

  def flushInput(self):
    self.ser.flushInput()

  def flushOutput(self):
    self.ser.flushOutput()

  def openDoor(self, outputfile):
    #self.ser.writeBytes('\x10')
    outputfile.write('success')
    outputfile.flush()

#  def get_key(key_id):
#    #TODO get the key form some database  
#    a = 'db 96 88 dd f1 e8 d8 87 15 18 6e 5a 94 14 3f 63 5b 16 9b 38 e5 1c ea c7 b4 30 ef 99 27 27 5c a5'
#    somekey = bytearray.fromhex(a)
#    return somekey
#  def chall_and_key(chall,key):
#    return  key & chall
#
#  if __name__ ==  '__main__':
#    port="/dev/ttyUSB0"
#    inter="1"
#    file="/dev/hwrng"
#
#    try:
#      ser = uopen(port)
#    except:
#      print("no serial")
#      sys.exit(1)
#
#    f = open(file,"rb")
#
#    while(True):
#      key_a=get_key(key_id)
#      ck=bytearray()
#      for s in key_a:
#        byte=f.read(1)
#        challange = struct.unpack('B', byte)[0]
#        ck.append(challange & s)
#        ser.write(byte)
#      chiper.update(ck)
#      answer = bytearray()
#      for  i in range(32):
#        answer.append(ser.read(1))
#      data1=list(answer)
#      data2=list(chiper.digest())
#      if (chiper.digest() == answer):
#        print("mr. gorbatschow tear down this wall\n http://www.youtube.com/watch?v=rLBsOz8lPhI")
#        ser.write(struct.pack('B',0x99))
#      else:
#        print("du kommst hier net rein \n http://www.youtube.com/watch?v=jLhYIqiJlEA")
#        ser.write(struct.pack('B',0x00))
#      time.sleep(10)

