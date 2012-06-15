#!/usr/bin/python

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

if __name__ == "__main__":
  main()
