#!/usr/bin/python

import os
import sys
import sqlite3

def createTable(connection):
  cursor = connection.cursor()
  cursor.execute("CREATE TABLE keys (key_id integer, key blob)")
  connection.commit()

def getKey(connection, keyid):
  cursor = connection.cursor()
  cursor.execute("SELECT key FROM keys WHERE key_id = ?", (keyid,))
  return cursor.fetchone()

def addKey(connection, keyid, key):
  cursor = connection.cursor()
  cursor.execute("INSERT INTO keys VALUES (?,?)", (keyid, key))
  connection.commit()

def main():
  if not os.path.exists(sys.argv[1]):
    print 'SQLite db file not found!'
    sys.exit(1);

  conn = sqlite3.connect(sys.argv[1]);

if __name__ == "__main__":
  main()
