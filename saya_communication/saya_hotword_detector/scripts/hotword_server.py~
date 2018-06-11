#!/usr/bin/env python
from saya_hotword_detector.srv import *
import rospy

#Hotword detection
import snowboydecoder

import time



#Global variable for hotword detection output
detected = False
interrupted = False
t=0
detector = snowboydecoder.HotwordDetector("/home/asimov16/IRA_V2_ws/src/saya_hotword_detector/scripts/resources/saya.pmdl", sensitivity=0.4, audio_gain=1)

def detected_callback():
	global detected
	#print ("Detected")

	#Assign the detected variable true.
	#The reason for using global variable is that we cannot return the outcome of the state over here. So, this will interrupt the callback once the hotword is detected and then in the main function by checking this variable , the outcome of the state will be set. 
	detected = True

	#detector.terminate()

def interrupt_callback():
	global interrupted,t

	
	start_time = time.time()
	
	t=(time.time() - start_time)+t
	print(t)
	
        #Timeout for the detection(10secs) and check the detection status
	if t<.0001 and not detected:
		interrupted=False
		return interrupted
	else:
		interrupted=True
		return interrupted

def hotword_detector(req):
	global detected,interrupted,t

	print ("detected=",detected)
	
	rospy.loginfo('Executing Hotword Detection')

	print("Tell the keyword to start conversation")
	
	
	
	detector.start(detected_callback=detected_callback,
               interrupt_check=interrupt_callback,
               sleep_time=0.03)
	
	rospy.sleep(1)
	
	if detected:
		t=0
		detected =False
		return True
	elif interrupted:
		t=0
		return False
	



def hotword_server():
	rospy.init_node('hotword_detector_server')

	

	s = rospy.Service('hotword_detector', hotword, hotword_detector)

	rospy.spin()


if __name__ == "__main__":
	hotword_server()
	
		
	
	

	
