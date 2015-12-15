#Raspberry Pi Code for Gloo Project
#Author: Alberto Tam Yong
#Partner: Stanley Chang
#Code based on sample code from OpenCV

import cv2
import numpy as np

#cv2.namedWindow("mask_red")
#cv2.namedWindow("mask_green")

img = np.zeros((512,512),np.uint8)

face_cascade = cv2.CascadeClassifier('/home/pi/Documents/python27/haarcascade_frontalface_default.xml')

cap = cv2.VideoCapture(0)

#Reduce video input size
cap.set(3,160)
cap.set(4,120)

print('Executing: color_track_4.py')
print('start')

#scan_image(): For testing and debuging
def scan_image():
	total = 0
		
	for i in range(0,160):
			for j in range(0,120):
				#mask_green[100+i,100+i] = 100
				if mask_green[i,j] >= 200:
					total += 1
					
	if total >= 20:
		print total
		
			
while(cap.isOpened()):

	# Take each frame
	_, frame = cap.read()
	
	if _ == True:
		# Convert BGR to HSV
		hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

		# define range of blue color in HSV
		#lower_blue = np.array([110,50,50])
		#upper_blue = np.array([130,255,255])

		#green
		lower_green = np.array([50,100,100])
		upper_green = np.array([70,255,255])
		
		#orange
		#hsv(25,100,100)
		#lower_orange = np.array([30,80,80])
		#upper_orange = np.array([50,100,100])
		
		#red
		lower_red = np.array([0,100,100])
		upper_red = np.array([5,255,255])
		
		# Threshold the HSV image to get only blue colors
		#mask = cv2.inRange(hsv, lower_blue, upper_blue)

		#green
		#mask = cv2.inRange(hsv,lower_green,upper_green)
		
		#orange
		#mask = cv2.inRange(hsv,lower_orange,upper_orange)
		
		mask_green = cv2.inRange(hsv,lower_green,upper_green)
		mask_red = cv2.inRange(hsv, lower_red, upper_red)
		
		moments_green = cv2.moments(mask_green)
		area_green = moments_green['m00']
		
		if area_green > 10000:
			x_green = int(moments_green['m10']/area_green)
			y_green = int(moments_green['m01']/area_green)
			print 'x: ' + repr(x_green) + '  y: ' + repr(y_green) + '  h: 10  t: 1'
			
		moments_red = cv2.moments(mask_red)
		area_red = moments_red['m00']
		
		if area_red > 50000:
			x_red = int(moments_red['m10']/area_red)
			y_red = int(moments_red['m01']/area_red)
			print 'x: ' +repr(x_red) + '  y: ' + repr(y_red) + '  h: 10  t: 2'
			
		gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
		
		faces = face_cascade.detectMultiScale(gray,1.3,5)
		for(x,y,w,h) in faces:
			print('x: '+repr(x)+'  y: '+repr(y)+ '  h: '+ repr(h)) + '  t: 0'
		
		# Bitwise-AND mask and original image
		#res = cv2.bitwise_and(frame,frame, mask= mask)
		
		#Display images with imshow
		#Not needed for actual project but good for debugging and development
		#cv2.imshow('frame',frame)
		#cv2.imshow('mask_red',mask_red)
		#cv2.imshow('mask_green',mask_green)
		k = cv2.waitKey(5) & 0xFF
		if k == 27:
			break

cv2.destroyAllWindows()