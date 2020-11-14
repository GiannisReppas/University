# python arrays.py rowsForArray1 rowsForArray2

import random
import sys
import os

numOfArgs = len(sys.argv)
args = list(sys.argv)

if numOfArgs != 3:
	print "Wrong number of arguements at command line"
	sys.exit()

# delete older input files
if os.path.isfile('input1.csv'):
	os.remove("input1.csv")
if os.path.isfile('input2.csv'):
	os.remove("input2.csv")

# dimensions of 2 arrays
array1Rows = int(args[1])
array2Rows = int(args[2])

# create first file
f1 = open("input1.csv","a")
for i in range(0,array1Rows):
	toWrite = str(random.randint(0,18446744073709551615) % 100)
	f1.write(toWrite)
	f1.write(",")
	toWrite = str(random.randint(0,18446744073709551615))
	f1.write(toWrite)
	f1.write("\n")
f1.close()

# create second file
f2 = open("input2.csv","a");
for i in range(0,array2Rows):
	toWrite = str(random.randint(0,18446744073709551615) % 100)
	f2.write(toWrite)
	f2.write(",")
	toWrite = str(random.randint(0,18446744073709551615))
	f2.write(toWrite)
	f2.write("\n")
f2.close()
