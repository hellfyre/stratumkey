import os
import sys
import sqlite3

class KeyDB:
  def __init__(self, dbfile):
    self.conn = sqlite3.connect(dbfile)
  
  def __del__(self):
    self.conn.close()

  def createTable(self):
    cursor = self.conn.cursor()
    cursor.execute("CREATE TABLE keys (key_id integer, key blob)")
    self.conn.commit()

  def getKey(self, keyid):
    cursor = self.conn.cursor()
    cursor.execute("SELECT key FROM keys WHERE key_id = ?", (keyid,))
    row = cursor.fetchone()
    if (row != None):
      return str(row[0])
    else:
      return None

  def addKey(self, keyid, key):
    cursor = self.conn.cursor()
    cursor.execute("INSERT INTO keys VALUES (?,?)", (keyid, buffer(key)))
    self.conn.commit()

  def close(self):
    self.conn.close()
