import os
import string
import numpy
import sys

# python nearDuplicate2.py -w shingle_size -t random_permutations_num -d directory_path
# w is for the length of a shingle
# t is for the number of random permutations
# d is for the directory which contains all the files to be added to the dataset

##################################### This function checks if the user gave a correct input from the command line ######################
def commandLineCheck():
	numOfArgs = len(sys.argv)
	args = list(sys.argv)

	if numOfArgs != 7:
		print("Wrong number of arguements at command line")
		sys.exit()

	if args[1] != "-w":
		print("Arguement number 1 must be -w")
		sys.exit()

	if args[3] != "-t":
		print("Arguement number 3 must be -t")
		sys.exit()

	if args[5] != "-d":
		print("Arguement number 5 must be -d")
		sys.exit()

	if int(args[2]) <= 0:
		print("w must be greater than 0")
		sys.exit()

	if int(args[4]) <= 0:
		print("t must be greater than 0")
		sys.exit()

	if not os.path.isdir(args[6]):
		print("Directory given does not exist")
		sys.exit()

	for (dirpath, dirnames, filenames) in os.walk(args[6]):
		break

	filenames2 = []
	for f in filenames:
		f2 = args[6] + "/" + f
		filenames2.append(f2)

	return int(args[2]), int(args[4]), filenames2
######################################################################################################################################

##################################### This function creates a dataset and also a shingle list for each file ##########################
def createDataset(w,filenames):

	print("Reading files...\n")
	dataset = []
	allFilesShingles = {}

	current_shingle = []
	for filename in filenames:
		fileShingles = []
		with open(filename,'r') as f:
			for line in f:
				line = line.translate(None,string.punctuation)
				line = line.lower()
				for word in line.split():
					current_shingle.append(word)
					if len(current_shingle) == w:
						if current_shingle not in dataset:
							dataset.append(list(current_shingle))
						if current_shingle not in fileShingles:
							fileShingles.append(list(current_shingle))
						del current_shingle[0]
			f.close()
		print(filename + " added to dataset")
		allFilesShingles.update( {filename : fileShingles} )

	if not dataset:
		print("Can't have an empty dataset")
		sys.exit()

	return dataset, allFilesShingles
######################################################################################################################################

##################################### This function creates a list with random permutations, using the given dataset #################
def createRandomPermutations(t, dataset):
	print("\nCreating random permutations...")
	allPermutations = []
	i=0
	while i < t:
		tempPermutation = numpy.random.permutation(dataset)
		allPermutations.append(tempPermutation)
		i += 1

	return allPermutations
######################################################################################################################################

##################################### This function creates a hash table for each permutation  #######################################
def createHashTables(randomPermutations):
	allHashTables = []
	for i in randomPermutations:
		currentHashTable = {}
		location = 0
		for j in i:
			currentHashTable.update( {tuple(j) : location} )
			location += 1
		allHashTables.append(currentHashTable)
	print("\nRandom permutations have been created\n")

	return allHashTables
######################################################################################################################################

##################################### This function computes the resemblance of two documents ########################################
def resemblance(filename1, filename2, fileShingles, hashTables):
	#### sketches ####
	docList1 = []
	docList2 = []
	for i in hashTables:

		currentIndex = 1
		currentMin = i[ tuple(fileShingles[filename1][0]) ]
		while currentIndex != len(fileShingles[filename1]):
			shingle = tuple(fileShingles[filename1][currentIndex])
			if currentMin > i[shingle]:
				currentMin = i[shingle]
			currentIndex += 1
		docList1.append( currentMin )

		currentIndex = 1
		currentMin = i[ tuple(fileShingles[filename2][0]) ]
		while currentIndex != len(fileShingles[filename2]):
			shingle = tuple(fileShingles[filename2][currentIndex])
			if currentMin > i[shingle]:
				currentMin = i[shingle]
			currentIndex += 1
		docList2.append( currentMin )	

	### resemblance ###
	intersection = list( set(docList1) & set(docList2) )
	union = list( set(docList1) | set(docList2) )
	resemblance = float(len(intersection)) / float(len(union))

	return resemblance
######################################################################################################################################

####################################### This function produces a dictinary for all sketches for every file ###########################
def calculateAllSketches(filenames, fileShingles, hashTables):

	allSketches = {}
	for filename in filenames:
		docList = []
		for i in hashTables:
			currentIndex = 1
			currentMin = i[ tuple(fileShingles[filename][0]) ]
			while currentIndex != len(fileShingles[filename]):
				shingle = tuple(fileShingles[filename][currentIndex])
				if currentMin > i[shingle]:
					currentMin = i[shingle]
				currentIndex += 1
			docList.append( currentMin )
		allSketches.update({filename : docList})

	return allSketches
######################################################################################################################################

################################ This function creates pairs of all documents with the biggest resemblance ###########################
def pairs(filenames, fileShingles, hashTables):

	# calculate all sketches for each file
	allSketches = calculateAllSketches(filenames, fileShingles, hashTables)

	# initiliaze 2d array of all resemblances
	pairs = []
	i = 0
	while i < len(filenames):
		toInsert = numpy.zeros(len(filenames))
		toInsert[i] = 1
		pairs.append(toInsert)

		i += 1

	# calculate all resemblances
	i = 0
	while i < len(filenames):

		j=0
		while j < len(filenames):

			if pairs[i][j] == 0:
				# resemblance
				intersection = list( set(allSketches[filenames[i]]) & set(allSketches[filenames[j]]) )
				union = list( set(allSketches[filenames[i]]) | set(allSketches[filenames[j]]) )
				resemblance = float(len(intersection)) / float(len(union))

				# r = resemblance(filenames[i], filenames[j], fileShingles, hashTables)
				pairs[i][j] = resemblance
				pairs[j][i] = resemblance

			j +=1

		i +=1

	# find pairs
	toReturn = []
	i = 0
	while i < len(filenames):

		currentMax = -1
		j=0
		while j < len(filenames):
			if i != j :
				if currentMax == -1 or pairs[i][j] > pairs[i][currentMax]:
					currentMax = j

			j += 1

		toReturn.append([filenames[i],filenames[currentMax]])
		i += 1

	return toReturn
######################################################################################################################################

# program starts here

# take information from the command line
# w --> length of shingles
# t --> number of random permutations
# filenames --> a list with the paths of all the files that will be used for the dataset
w,t,filenames = commandLineCheck()

# dataset --> a list of shingles from all the files (contains no duplicates)
# fileShingles --> a list, of which each element is a small dataset which contains all the shingles for each file
dataset, fileShingles = createDataset(w,filenames)

# randomPermutations --> a list of t lists (each list represents a random permutation)
randomPermutations = createRandomPermutations(t, dataset)

# hashTables --> a list of t hash tables (each hash table represents a random permutation, shingles->keys - indexes->values)
hashTables = createHashTables(randomPermutations)

########################## DOCUMENT RESEMBLANCE CALCULATION ####################
while True:
	print("------------------------- Menu -----------------------------------")
	print("/resemblance filename1 filename2 --> give 2 document names to check their resemblance")
	print("/pairs --> prints pairs of closely resembling documents in the current filder")
	print("/exit --> end of program\n")
	userInput = raw_input().split()

	if (userInput[0] == '/resemblance') and (len(userInput) == 3):
		if userInput[1] not in fileShingles:
			print("File 1 does not exist")
		elif userInput[2] not in fileShingles:
			print("File 2 does not exist")
		else:
			print( resemblance(userInput[1], userInput[2], fileShingles, hashTables) )

	elif (userInput[0] == '/pairs') and (len(userInput) == 1):
		myPairs = pairs(filenames, fileShingles, hashTables)
		for i in myPairs:
			print(i)

	elif (userInput[0] == '/exit') and (len(userInput) == 1):
		break

	else:
		print("Wrong input\n\n")