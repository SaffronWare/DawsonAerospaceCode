# I started off trying to use AI for this, but it didnt work and it pissed me off, so instead I wrote horrible manual code for rendering as the python
# 3d engins are actually fucking cheeks to use.

import pygame 
import serial
import time 
from math import sin,cos
import csv
import struct
from math import sin, cos

TESTING = False

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

rudder_origin = (4.2,0,0.8)
elevator_origin = (3.5,0,0)
aileron_origin = (-1.65, 0,0)


# make sure the port used in serialCom line is the one connected or nano every
#ports = list_ports.comports()
#for port in ports:
#    print(port)


# if line starts with DBG, its a data line.
key = "DBG"
formatting = ["roll", "pitch", "yaw", "rudder", "aileron", "elevator"]

serialCom = None

if not TESTING:
    serialCom = serial.serial_for_url('COM9', 115200, timeout=1) # make sure baud rate matches arduino

    # magic code that resets arduino (turns connection off -> back on -> resets and in between clears serial)
    serialCom.setDTR(False)
    time.sleep(1)
    serialCom.flushInput()
    serialCom.setDTR(True)

    print("serialized...")
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

def normalf(v1,v2,v3):
    x1,y1,z1 = [v1[i]- v2[i] for i in range(3)]
    x2,y2,z2 = [v2[i] - v3[i] for i in range(3)]
    x = y1*z2 - z1*y2
    y = z1*x2 - x1*z2
    z = x1*y2 - y1*x2
    f = (x**2+y**2+z**2)**(-1/2)

    return (x*f,y*f,z*f)






# object x ->
def render(vertices,window):
    projected_points = []
    normals = []
    for vertex in vertices:
        p_vertex = []
        for point in vertex:
            #print(vertex)
            dist_buff = 500/(point[0]-16) # dividng by x
            x = ((point[1]) * dist_buff) + window_size[0]//2
            y = ((point[2] - 3) * dist_buff) + window_size[1]//2
            p_vertex.append((x,y))
        normal = normalf(vertex[0],vertex[1],vertex[2])
        normals.append(normal)
        projected_points.append((p_vertex, abs(sum(point[0] - 12 for point in vertex))))

    pp = [(p[0], normal) for p, normal in sorted(zip(projected_points,normals), key=lambda x : x[0][1])]


    for vertex,normal in pp[::-1]:

        
        color_factor = normal[0] + normal[1] + normal[2]
        color_factor/=2
        color_factor = abs(color_factor)
        color_factor = min(color_factor,1)
        color_factor = max(0,color_factor)
        
        pygame.draw.polygon(window, (200*color_factor,200*color_factor,200*color_factor), vertex)


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


    """
    render(plane_frame, window)
    render(rudder_frame,window)
    render(elevator_frame,window)
    render(ar_frame, window)
    render(al_frame, window)
    """
    render(plane_frame + rudder_frame +ar_frame + elevator_frame + al_frame, window)



    
    data_bytes = b""
    
    if TESTING:
        data_bytes = f"DBG {45 * (sin(2*time.time()) + 1)/2} {45 * (sin(time.time()) + 1)/2} {0 * (sin(time.time()) + 1)/2} {45 * (sin(3*time.time()) + 1)/2} {45 * (sin(2.5*time.time()) + 1)/2} {45 * (sin(time.time()) + 1)/2}".encode()
        time.sleep(1/60)
    else:
        data_bytes = serialCom.readline()
        
    decoded_data= data_bytes.decode('utf-8').strip().split()
    print(decoded_data)
    if decoded_data and decoded_data[0] == key:
        decoded_data = [float(v) for v in decoded_data[1:]]
        #print(decoded_data)

        

        roll = decoded_data[formatting.index("roll")]
        yaw = decoded_data[formatting.index("yaw")] * 0 # yaw is unreliable
        pitch = decoded_data[formatting.index("pitch")]

        rudderAngle = decoded_data[formatting.index("rudder")]
        aileronAngle = decoded_data[formatting.index("aileron")]
        elevatorAngle = decoded_data[formatting.index("elevator")]

        rudder_frame = rotate_yaw(rudder, rudderAngle * 3.14159 / 180.0, rudder_origin)
        elevator_frame = rotate_pitch(elevator, elevatorAngle * 3.14159 / 180.0, elevator_origin)
        ar_frame = rotate_pitch(ar, aileronAngle * 3.14169 / 180, aileron_origin)
        al_frame = rotate_pitch(al, -aileronAngle * 3.14159 / 180, aileron_origin)

        plane_frame = rotate_pitch(plane, pitch * 3.14159 / 180.0, (0,0,0))
        elevator_frame = rotate_pitch(elevator_frame, pitch * 3.14159 / 180.0, (0,0,0))
        rudder_frame = rotate_pitch(rudder_frame, pitch * 3.14159 / 180.0, (0,0,0))
        ar_frame = rotate_pitch(ar_frame, pitch * 3.14159 / 180.0, (0,0,0))
        al_frame = rotate_pitch(al_frame, pitch * 3.14159 / 180.0, (0,0,0))

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


        
    pygame.display.flip()
    
    

