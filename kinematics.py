import math
from socket import *
import time
counter =0
angle1 = [0]
angle2 = [0]

#X====  1700  -1300
#Y====  4300  -4300


address = ('169.254.88.53', 8888)  # match arduino IP + port
client_socket = socket(AF_INET, SOCK_DGRAM)
# print(address)
client_socket.settimeout(10)  # wait up to 5 seconds

# Motor parameters
r = 600 # Distance from center of bot to motor
l = 700 # Distance between motors

# Forward kinematics: (theta1, theta2) -> (x, y)
#   Returns [x, y]
def forward(theta1, theta2):
    theta1 *= math.pi/180 # Convert to radians
    theta2 *= math.pi/180
    x = r * (math.cos(theta1) + math.cos(theta2))
    y = r * (math.sin(theta1) + math.sin(theta2))
    return [x, y]




def inverse(displacement_x, displacement_y):
    if displacement_x == 0 or displacement_y==0:
        displacement_magnitude = abs(math.sqrt(displacement_x**2 + displacement_y**2))
        displacement_angle = math.atan2(abs(displacement_y), displacement_x)
        
        motor_angle = displacement_magnitude / 90.0
        
        motor1_angle = motor_angle / 2 * 180 / math.pi
        motor2_angle = motor_angle / 2 * 180 / math.pi
        
        if displacement_x > 0:
            motor2_angle = -motor2_angle
        elif displacement_x < 0:
            motor1_angle = -motor1_angle
        
        if displacement_y < 0:
            motor1_angle = -motor1_angle
            motor2_angle = -motor2_angle
    
    else:
        
        length1 = len(angle1)
        length2 = len(angle2)        

        diagonal_magnitude = math.sqrt(displacement_x**2 + displacement_y**2)
        diagonal_angle = math.atan2(displacement_y, displacement_x)
        
        motor_angle = diagonal_magnitude / 90.0

        if displacement_x > 0 and displacement_y > 0:
            motor1_angle = motor_angle / 2 * 180 / math.pi
            motor2_angle = angle2[length2-1]
        elif displacement_x < 0 and displacement_y > 0:
            motor2_angle = 1*motor_angle / 2 * 180 / math.pi
            motor1_angle = angle1[length1-1]
        elif displacement_x > 0 and displacement_y < 0:
            motor1_angle = motor_angle / 2 * 180 / math.pi
            motor2_angle = angle2[length2-1]
        else:
            motor2_angle = -1 * motor_angle / 2 * 180 / math.pi
            motor1_angle = angle1[length1-1]            
            

    #print(angle1)
    angle1.append(motor1_angle)    
    angle2.append(motor2_angle)  
 

        
    return motor1_angle, motor2_angle



filepath = '/Users/amithnalherath/Library/Mobile Documents/com~apple~CloudDocs/Desktop/Curtin/Internship/Tub inspection/cord.txt'

while(True):

    time.sleep(1)
    with open(filepath) as fp:
        line = fp.readline()
        while line:
            line = fp.readline()
            txt = "{}".format(line.strip())
            a = txt.split()
            if 3 < len(a):  # check the array location is availbe
                F = a[2].split("F", 1)
                A = a[3].split("A", 1)
            if a:
                if (a[0])[0] == 'X':
                    x = a[0].split("X", 1)
                    y = a[1].split("Y", 1)
                    coordinate = inverse(float(x[1]),float(y[1]))
                    a = round(coordinate [0],2)
                    b = round(coordinate [1],2)
                    print(a,b,F[1],A[1])
                    client_socket.sendto(('a' + str(a) + ' b' + str(b)+ ' F' + str(F[1]) + ' A' + str(A[1])).encode(), address)
                else:
                    if (a[0])[0] == "A":
                        a = txt.split()
                        gr = a[1]
                        delay = a[2]
                        if gr == "TIME":
                            # print("TIME :" , (int(del_pre)+int(delay))/1000 )
                            full_time = (int(del_pre)+int(delay))/1000 + full_time
                            del_pre = 0
                        time.sleep(int(delay)/1000)
    print();  
  



                                       

  







