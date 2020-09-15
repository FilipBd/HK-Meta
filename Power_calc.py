from math import*

#Variables

tourqe = 10.5
rpm = 9000
r_platform = 0.85
mass = 200
angle = 45
r_pully = 0.05

#Calc 1 

s = 2*pi*r_platform*angle/360 				#Arc length of rail

varv = s/(r_pully*2*pi)						#Revolutions of pully

#print("varv =", varv)

rad = varv*2*pi 							#Total radians

#print("rad =", rad)

#Calc 2

Tourqe_person = mass*9.82*r_pully			#Torque of person

Power_motor_chosen = tourqe*2*pi*rpm/60		#Torque of chosen motor

Power_motor_needed = Tourqe_person*rad 		#Torque needed

Power_factor = Power_motor_chosen/Power_motor_needed 		#Torque factor

print(" Power chosen =", Power_motor_chosen)
print(" Power needed =", Power_motor_needed)
print(" Factor =",Power_factor)






