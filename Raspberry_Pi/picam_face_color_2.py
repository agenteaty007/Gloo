#rpi code
#picamera
#Raspberry Pi 2 for best performance

from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2
import time
import numpy as np

#cv2.namedWindow("mask_red")
#cv2.namedWindow("mask_green")

img = np.zeros((512,512),np.uint8)

face_cascade = cv2.CascadeClassifier('/home/pi/Documents/python27/haarcascade_frontalface_default.xml')

camera = PiCamera()
camera.resolution = (320,240) #lower resolution if you need to improve performance
camera.framerate = 30
rawCapture = PiRGBArray(camera, size = (320,240))
#cap = cv2.VideoCapture(0)
time.sleep(0.2)

print('Executing: picam_face_color_2.py')
print('start')

def scan_image():
	total = 0
		
	for i in range(0,160):
			for j in range(0,120):
				#mask_green[100+i,100+i] = 100
				if mask_green[i,j] >= 200:
					total += 1
					
	if total >= 20:
		print total
		
			
#while(cap.isOpened()):
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port = True):

	# Take each frame
	#_, frame = cap.read()
	image = frame.array
	frame = image
	live = image
	
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
	upper_red = np.array([4,255,255])
	
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
	
	#Detecting green objects
	#10000 pixel is the size threshold to detect only large items
	if area_green > 10000:
		x_green = int(moments_green['m10']/area_green)
		y_green = int(moments_green['m01']/area_green)
		print 'x: ' + repr(x_green) + '  y: ' + repr(y_green) + '  h: 10  t: 1'
		
	moments_red = cv2.moments(mask_red)
	area_red = moments_red['m00']
	
	#Detecting red objects
	#50000 pixel is the size threshold to detect only large items
	if area_red > 50000:
		x_red = int(moments_red['m10']/area_red)
		y_red = int(moments_red['m01']/area_red)
		print 'x: ' +repr(x_red) + '  y: ' + repr(y_red) + '  h: 10  t: 2'
		
	#Image on Gray for Face Detection
	gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
	
	faces = face_cascade.detectMultiScale(gray,1.3,5)
	for(x,y,w,h) in faces:
		cv2.rectangle(live,(x,y),(x+w,y+h),(255,0,0),2)
		print('x: '+repr(x)+'  y: '+repr(y)+ '  h: '+ repr(h)) + '  t: 0'
	
	# Bitwise-AND mask and original image
	#res = cv2.bitwise_and(frame,frame, mask= mask)
	
	cv2.imshow('live',live)
	cv2.imshow('mask_red',mask_red)
	cv2.imshow('mask_green',mask_green)
	k = cv2.waitKey(5) & 0xFF
	if k == 27:
		break
	rawCapture.truncate(0)

cv2.destroyAllWindows()