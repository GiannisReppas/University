#! /bin/bash

# set variables for every stat you need to print
total_bytes_written=0
total_bytes_read=0
total_files_written=0
total_files_read=0
total_clients_that_left_the_system=0
total_clients_connected=0
counter=0 # list of the clients that had at least one connection with another client
array=()

# for every input read from log_files
while read p
do
	# each line in log file has a special character at the beggining
	# this character specifies the type of the stat
	# depending on the type of the stat, do the right move

	if [ "${p:0:1}" = "1"  ]
	then
		total_bytes_written=$((total_bytes_written+${p:3}))
	elif [ "${p:0:1}" = "2" ]
	then
		total_files_written=$((total_files_written+${p:3}))
	elif [ "${p:0:1}" = "3" ]
	then
		total_bytes_read=$((total_bytes_read+${p:3}))
	elif [ "${p:0:1}" = "4" ]
	then
		total_files_read=$((total_files_read+${p:3}))
	elif [ "${p:0:1}" = "5" ]
	then
		# get the last word of the line( yes or no)
		for i in $p
		do
			j=1
		done

		# if the last word is a yes, then it means that the two clients had a successfull connection
		if [ "${i:0:1}" = "y" ]
		then
			# so, increase the counter of this kind of clients
			total_clients_connected=$((total_clients_connected+1))

			# put the ids of those 2 clients into the list
			# if the were already in the list beqause of another transactiom, ignore them
			j=0
			for i in $p
			do
				if [ $j = "1" ] || [ $j = "2" ]
				then

					check=1
					for k in "${array[@]}"
					do
						if [ $((i)) -eq $((k)) ]
						then
							check=0
							break
						fi
					done

					if [ $check = "1" ]
					then
						array[counter]=$i
						counter=$((counter+1))
					fi

				fi

				j=$((j+1))
			done

		fi

	else
		total_clients_that_left_the_system=$((total_clients_that_left_the_system+1))
	fi
done

echo "Total bytes written -->  '$total_bytes_written' "

echo "Total bytes read --> '$total_bytes_read' "

echo "Total files written --> '$total_files_written' "

echo "Total files read --> '$total_files_read' "

echo "Total clients that left the system --> '$total_clients_that_left_the_system' "

# while printing the clients that had at least one successfull connection,
# find also the min and max id
min=${array[0]}
max=${array[0]}
echo "Total clients connected --> '$total_clients_connected' "
echo "These clients' ids are:"
for i in "${array[@]}"
do
	echo "$i"

	if [ $((min)) -gt $((i)) ]
	then
		min=$((i))
	fi

	if [ $((max)) -lt $((i)) ]
	then
		max=$((i))
	fi
done
echo " min id --> $min"
echo " max id --> $max"
