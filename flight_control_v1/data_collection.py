# I started off trying to use AI for this, but it didnt work and it pissed me off, so instead I wrote horrible manual code for rendering as the python
# 3d engins are actually fucking cheeks to use.

import pygame 
import serial
import time 
from math import sin,cos
import csv
import struct
from math import sin, cos

TESTING = True

window_size = (500,500)


def load_mesh(path):
    with open(path, "rb") as file:
        print(file.read(80).decode("ascii"))
        num_tris = struct.unpack("<I", file.read(4))[0]
        print(num_tris)

        triangles = []
        for i in range(num_tris):
            # normal = 3x32 bit float -> 12 byte
            # 3 vertex 1 = 12 byte
            # v2 = 12 byte
            # v3 =12 byte
            # attributes =2 byte
            # total =12+12+12+12+2 = 50 byte
            #triangle_data = file.read(50)
            components = [struct.unpack("<f", file.read(4))[0] for i in range(12)] # nx,ny,nz,vx,vy,vz
            file.read(2) # skip last2 attributes

            triangles.append((components[3:6], components[6:9], components[9:12])) # only give verex data [vx,vy,vz]
        return triangles # for now im ignoring normals





plane = load_mesh("plane_model//plane.stl")
rudder = load_mesh("plane_model//Rudder.stl")
elevator = load_mesh("plane_model//Elevator.stl")
ar = load_mesh("plane_model//aileronR.stl")
al = load_mesh("plane_model//aileronL.stl")


# make sure the port used in serialCom line is the one connected or nano every
#ports = list_ports.comports()
#for port in ports:
#    print(port)


# if line starts with DBG, its a data line.
key = "DBG"
formatting = ["roll", "pitch", "yaw", "rudder", "aileron", "elevator"]



if not TESTING:
    serialCom = serial.serial_for_url('loop://', 115200, timeout=1) # make sure baud rate matches arduino

    # magic code that resets arduino (turns connection off -> back on -> resets and in between clears serial)
    serialCom.setDTR(False)
    time.sleep(1)
    serialCom.flushInput()
    serialCom.setDTR(True)

window = pygame.display.set_mode(window_size)



def rotate_pitch(vertices,angle, origin):
    output = []
    for vertex in vertices:
        p_vertex = []
        for point in vertex:
            ox = point[0] - origin[0]
            oz = point[2] - origin[2]

            c = cos(-angle)
            s = sin(-angle)

            p_vertex.append((ox*c - oz*s + origin[0],point[1], ox*s + oz*c + origin[2]))
        output.append(p_vertex)
    return output


def rotate_roll(vertices,angle, origin):
    output = []
    for vertex in vertices:
        p_vertex = []
        for point in vertex:
            oy = point[1] - origin[1]
            oz = point[2] - origin[2]

            c = cos(angle)
            s = sin(angle)

            p_vertex.append((point[0], oy*c - oz*s + origin[1], oy*s + oz*c + origin[2]))
        output.append(p_vertex)
    return output


def rotate_yaw(vertices,angle, origin):
    output = []
    for vertex in vertices:
        p_vertex = []
        for point in vertex:
            ox = point[0] - origin[0]
            oy = point[1] - origin[1]

            c = cos(angle)
            s = sin(angle)

            p_vertex.append((ox*c - oy*s + origin[0], ox*s + oy*c + origin[1], point[2]))
        output.append(p_vertex)
    return output



"""
def rotate_roll(vertices, origin):
    pass 

def rotate_yaw(vertices, origin)
"""


# object x ->
def render(vertices,window):
    projected_points = []
    for vertex in vertices:
        p_vertex = []
        for point in vertex:
            #print(vertex)
            dist_buff = 500/(point[0]-12) # dividng by x
            x = ((point[1]) * dist_buff) + window_size[0]//2
            y = ((point[2] - 2) * dist_buff) + window_size[1]//2
            p_vertex.append((x,y))
        projected_points.append(p_vertex)

    for vertex in projected_points:
        pygame.draw.line(window, (200,200,200), vertex[0], vertex[1])
        pygame.draw.line(window, (200,200,200), vertex[1], vertex[2])
        pygame.draw.line(window, (200,200,200), vertex[2], vertex[0])


plane_frame = plane
rudder_frame = rudder
elevator_frame = elevator
ar_frame = ar 
al_frame = al

    
running = True
while running:
    window.fill((0,0,0))
    """x is depth!!!"""
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running= False


    render(plane_frame, window)
    render(rudder_frame,window)
    render(elevator_frame,window)
    render(ar_frame, window)
    render(al_frame, window)



    try:
        data_bytes = b""
        if TESTING:
            data_bytes = f"DBG 0 0 {45 * (sin(time.time()) + 1)/2} 0 0 0".encode()
            time.sleep(1/60)
        else:
            data_bytes = serialCom.readline()
           
        decoded_data= data_bytes.decode('utf-8').strip().split()
        if decoded_data[0] == key:
            decoded_data = [float(v) for v in decoded_data[1:]]

            

            roll = decoded_data[formatting.index("roll")]
            yaw = decoded_data[formatting.index("yaw")]
            pitch = decoded_data[formatting.index("pitch")]

            rudderAngle = decoded_data[formatting.index("rudder")]
            aileronAngle = decoded_data[formatting.index("aileron")]
            elevatorAngle = decoded_data[formatting.index("elevator")]

            plane_frame = rotate_pitch(plane, pitch * 3.14159 / 180.0, (0,0,0))
            elevator_frame = rotate_pitch(elevator, pitch * 3.14159 / 180.0, (0,0,0))
            rudder_frame = rotate_pitch(rudder, pitch * 3.14159 / 180.0, (0,0,0))
            ar_frame = rotate_pitch(ar, pitch * 3.14159 / 180.0, (0,0,0))
            al_frame = rotate_pitch(al, pitch * 3.14159 / 180.0, (0,0,0))

            plane_frame = rotate_roll(plane_frame, roll * 3.14159 / 180.0, (0,0,0))
            elevator_frame = rotate_roll(elevator_frame, roll * 3.14159 / 180.0, (0,0,0))
            rudder_frame = rotate_roll(rudder_frame, roll * 3.14159 / 180.0, (0,0,0))
            ar_frame = rotate_roll(ar_frame, roll * 3.14159 / 180.0, (0,0,0))
            al_frame = rotate_roll(al_frame, roll * 3.14159 / 180.0, (0,0,0))
            
            plane_frame = rotate_yaw(plane_frame, yaw * 3.14159 / 180.0, (0,0,0))
            elevator_frame = rotate_yaw(elevator_frame, yaw * 3.14159 / 180.0, (0,0,0))
            rudder_frame = rotate_yaw(rudder_frame, yaw * 3.14159 / 180.0, (0,0,0))
            ar_frame = rotate_yaw(ar_frame, yaw * 3.14159 / 180.0, (0,0,0))
            al_frame = rotate_yaw(al_frame, yaw * 3.14159 / 180.0, (0,0,0))


    except Exception as e:
        print(f"error : {e}")
        
    pygame.display.flip()
    
    

