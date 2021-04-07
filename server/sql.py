import sqlite3

conn = sqlite3.connect("sensors_data.db")
curs = conn.cursor()
#curs.execute("CREATE TABLE sensors \
#    (date TEXT NOT NULL, time TEXT NOT NULL, light_1 INTEGER, \
#        temp_water REAL, tds INTEGER, co2 INTEGER)")

print(curs.execute("SELECT * FROM sensors").fetchall())
conn.commit()
conn.close()
