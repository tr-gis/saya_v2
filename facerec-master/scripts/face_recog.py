#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import String
import cv2, sys, numpy, os
import imutils

size = 2
fn_haar = '/home/asimov/IRA_V2_ws/src/facerec-master/scripts/haarcascade_frontalface_default.xml'
fn_dir = '/home/asimov/IRA_V2_ws/src/facerec-master/scripts/att_faces'
detection_height = 60  
frame_count_thres=50
predicion_thres=500
show_output=True #False

num_of_faces=0
frame_count=0
face_id=""


if __name__ == '__main__':
	try:
		haar_cascade = cv2.CascadeClassifier(fn_haar)
		webcam = cv2.VideoCapture(0)
		pub = rospy.Publisher('face_rec', String, queue_size=10)
		rospy.init_node('face_rec', anonymous=True)

		# Part 1: Create fisherRecognizer
		print('Training...')
		# Create a list of images and a list of corresponding names
		(images, lables, names, id) = ([], [], {}, 0)

		# Get the folders containing the training data
		for (subdirs, dirs, files) in os.walk(fn_dir):

			# Loop through each folder named after the subject in the photos
			for subdir in dirs:
				names[id] = subdir
				subjectpath = os.path.join(fn_dir, subdir)

				# Loop through each photo in the folder
				for filename in os.listdir(subjectpath):

					# Skip non-image formates
					f_name, f_extension = os.path.splitext(filename)
					if(f_extension.lower() not in ['.png','.jpg','.jpeg','.gif','.pgm']):
						print("Skipping "+filename+", wrong file type")
						continue
					path = subjectpath + '/' + filename
					lable = id

					# Add to training data
					images.append(cv2.imread(path, 0))
					lables.append(int(lable))
        			id += 1
		(im_width, im_height) = (112, 92)

		# Create a Numpy array from the two lists above
		(images, lables) = [numpy.array(lis) for lis in [images, lables]]

		# OpenCV trains a model from the images
		# NOTE FOR OpenCV2: remove '.face'
		model = cv2.face.createFisherFaceRecognizer()
		model.train(images, lables)

		while not rospy.is_shutdown():
			#hello_str = "hello world %s" % rospy.get_time()
			#pub.publish(hello_str)
			# Loop until the camera is working
			rval = False
			while(not rval):
				# Put the image from the webcam into 'frame'
				(rval, frame) = webcam.read()
				if(not rval):
					print("Failed to open webcam. Trying again...")
			
			#Rotate the frame
			frame = imutils.rotate(frame,90)

			# Flip the image (optional)
			frame=cv2.flip(frame,1,0)

			# Convert to grayscalel
			gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

			# Resize to speed up detection (optinal, change size above)
			mini = cv2.resize(gray, (int(gray.shape[1] / size), int(gray.shape[0] / size)))

			# Detect faces and loop through each one
			faces = haar_cascade.detectMultiScale(mini)
			num_of_faces=len(faces)	

			if(num_of_faces>0):
				frame_count+=1
				closest_face=[]
				h_thres=0
				if(frame_count>frame_count_thres):
					frame_count=0
					if(num_of_faces==1):
						#print (faces)
						(x, y, w, h) = [v * size for v in faces[0]]
						closest_face=faces[0]
						#print closest_face
						#print (x, y, w, h)
					else:
			 			for i in range(len(faces)):
							face_i = faces[i]
							# Coordinates of face after scaling back by `size`
							(x, y, w, h) = [v * size for v in face_i]
							if(h_thres<=h):
								closest_face=face_i
								h_thres=h
						(x, y, w, h)=[i*size for i in closest_face]
						#print [i*2 for i in closest_face]
					if (h>detection_height):
						cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 3)
						face = gray[y:y + h, x:x + w]
						face_resize = cv2.resize(face, (im_width, im_height))
						# Try to recognize the face
						prediction = model.predict(face_resize)
						# [1] Write the name of recognized face
						if prediction[1]<predicion_thres:
							cv2.putText(frame,'%s - %.0f' % (names[prediction[0]],prediction[1]),(x-10, y-10), cv2.FONT_HERSHEY_PLAIN,1,(0, 255, 0))
							face_id=names[prediction[0]]
							pub.publish(face_id)
						else:
							cv2.putText(frame,'Unknown',(x-10, y-10), cv2.FONT_HERSHEY_PLAIN,1,(0, 255, 0))
							face_id="Unknown"
							pub.publish(face_id)
			else:
				frame_count=0
				face_id="None"
				pub.publish(face_id)
				
	

			# Show the image and check for ESC being pressed
			if(show_output):
				cv2.imshow('OpenCV', frame)
				key = cv2.waitKey(10)
				if key == 27:
					break

	except rospy.ROSInterruptException:
		pass

