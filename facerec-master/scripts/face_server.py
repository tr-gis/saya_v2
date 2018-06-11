#!/usr/bin/env python

import rospy
from face_rec.srv import *
from std_msgs.msg import String

def handle_face_server(req):
	#print "Request received : %s "%(req.who)
	return serviceResponse(face_pred)

def sub_callback(data):	
	global face_pred
	face_pred=data.data
	rospy.loginfo(rospy.get_caller_id() + "I heard %s", face_pred)

def face_server():
	rospy.init_node('face_server')
	s = rospy.Service('face_server', service, handle_face_server)
	rospy.Subscriber("face_rec", String, sub_callback)
	print "face_server running"
	rospy.spin()

if __name__ == "__main__":
    face_server()
