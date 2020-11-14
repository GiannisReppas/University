#! /bin/bash

#################################### command line check #########################################
if [ $# -ne 4 ]
then
	echo "wrong number of arguements"
fi

a2=$2
if [ $((a2)) -le 0 ]
then
	echo "second arguement not a number"
fi
a3=$3
if [ $((a3)) -le 0 ]
then
	echo "third arguement not a number"
fi
a4=$4
if [ $((a4)) -le 0 ]
then
	echo "fourth arguement not a number"
fi
################################################################################################

################################### create dir_name (if it does not exist) #####################
a1=$1
if [ ! -d $a1 ]
then
	mkdir $a1
fi
################################################################################################

################################### create random directory names ##############################
i=0
while [ $i -lt $a3 ]
do
	dir_names[$i]="$(./random_dir_name)"
	sleep 1
	i=$((i+1))
done
################################################################################################

################################## create directories ##########################################
i=0
currentLevel=1
path=$a1
temp=$((a3-1))
while [ $i -lt $a3 ]
do
	while [ -d "$path/${dir_names[$l]}" ]
	do
		if [ "$l" == "$temp" ]
		then
			l=0
		else
			l=$((l+1))
		fi
	done

	path="$path/${dir_names[$l]}"

	path_array[$i]=$path
	mkdir ${path_array[$i]}

	if [ "$l" == "$temp" ]
	then
		l=0
	else
		l=$((l+1))
	fi

	i=$((i+1))
	if [ $currentLevel -eq $a4 ]
	then
		currentLevel=1
		path=$a1
	else
		currentLevel=$((currentLevel+1))
	fi
done
################################################################################################

################################### create random file names ###################################
i=0
while [ $i -lt $a2 ]
do
	file_names[$i]="$(./random_dir_name)"
	sleep 1
	i=$((i+1))
done
################################################################################################

################################## create files ################################################
i=0
j=0
temp=$((a3-1))
while [ $i -lt $a2 ]
do
	touch "${path_array[$j]}/${file_names[$i]}"
	echo "$(./random_text)" >> "${path_array[$j]}/${file_names[$i]}"
	sleep 1

	i=$((i+1))

	if [ "$j" == "$temp" ]
	then
		j=0
	else
		j=$((j+1))
	fi
done
################################################################################################
