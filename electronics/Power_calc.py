from math import*

#Variables

def rad2degree(rad):
	degree = rad*180/pi
	return degree
	

def degree2rad(degree):
	rad = degree*pi/180
	return rad

#tourqe = 10.5
#rpm = 9000
r_platform = 0.85
mass = 300
angle = 45
r_pully = 0.05

degree_acc = 200									#degree/sec^2

Motor_power = 3000									#power of motor (W)	

ang_acc = degree2rad(degree_acc)							#rad/sec^2

lin_acc = ang_acc*r_platform

#Calc 1 

s = 2*pi*r_platform*angle/360 				#Arc length of rail

varv = s/(r_pully*2*pi)						#Revolutions of pully

print("revs required =", varv)

#rad = varv*2*pi 							#Total radians

#print("rad =", rad)

#Calc 2

F_req = mass*lin_acc + mass*9.82

tourqe_req = F_req*r_pully

print("Force required =",F_req)

print("torque required =",tourqe_req)

omega = Motor_power/tourqe_req

#print(omega)

varv_sec = omega/(2*pi)

print("revs achived =",varv_sec)

factor = varv_sec/varv

print("gear ratio possible =",factor)

