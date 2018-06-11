#!/bin/bash
#author info : created by raj 
#contact : raj@asimovrobotics.com

#Dependencies:
#	*)gnu-screen (Tested with Screen version 4.03.01 (GNU) 28-Jun-15)
#	*)bash version >=4.0 (Tested with version 4.3.48(1))


### How to use this script ####

#Enter the session names to kill, as an element in array sessions
#Example : arr =("running_session_name")
#To get a list of running sessions use command :$screen -ls

#Do note that this has been test with named sessions only.


declare -a sessions=("roscore" "listener" "talker")

for i in "${sessions[@]}"
do
   screen -S $i -X quit
done

echo "The following are sessions currently running : "
screen -ls
