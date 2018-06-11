#!/bin/bash
#author info : created by raj 
#contact : raj@asimovrobotics.com

#Dependencies:
#	*)gnu-screen (Tested with Screen version 4.03.01 (GNU) 28-Jun-15)
#	*)bash version >=4.0 (Tested with version 4.3.48(1))


###			 How to use this script 		####

#For each session create two variables 1)sessionnum and 2)command_number
#Note the multiple commands are split with \n escape character

#Example :
#session10="some_name"
#command_10="command1\ncommand2\n.. commandn\n"

#To add this script to startup:
#open crontab with :$crontab -e
#add the line to the end : @reboot sleep 10 && /path/to/start.sh
#save file 

session1="roscore"
command_1="roscore\n"

session2="speech_recognition"
command_2='ssh pi@raspberrypi\nsleep 3\nexport GOOGLE_APPLICATION_CREDENTIALS=/home/pi/catkin_ws/src/speech-test-a654be8d4669.json\nrosrun saya_speech_recognition speech_server.py\n'

session3="hotword_detection"
command_3='ssh pi@raspberrypi\nsleep 3\nrosrun saya_hotword_detector hotword_server.py\n'

session4="speech_database"
command_4='\nsleep 3\nrosrun saya_speech_database saya_database_server.py\n'

session5="text_speech"
command_5='\nsleep 3\nrosrun saya_text_speech saya_text_speech_server.py\n'

session6="IRA_taskflow"
command_6='\nsleep 3\nrosrun saya_states ira_speech_service_architecture.py\n'



declare -A sessions=( [$session1]=$command_1 [$session2]=$command_2 [$session3]=$command_3 )

if ! which screen > /dev/null; then
	echo “Please install gnu-screen package”
	exit
fi

for session in "${!sessions[@]}"
do 
	screen -dmS $session
	screen -S $session -X stuff "${sessions[$session]}"	
done

echo "The following sessions are running:"
screen -ls
