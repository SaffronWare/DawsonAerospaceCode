from serial.tools import list_ports 
import serial
import time 
import csv 



# make sure the port used in serialCom line is the one connected or nano every
ports = list_ports.comports()
for port in ports:
    print(port)

# if line starts with DBG, its a data line.
key = "DBG"
formatting = ["roll", "pitch", "yaw", "rudder", "elevator", "aileron"]

# were gonna write our data to this csv


serialCom = serial.Serial('COM3', 115200) # make sure baud rate matches arduino

# magic code that resets arduino (turns connection off -> back on -> resets and in between clears serial)
serialCom.setDTR(False)
time.sleep(1)
serialCom.flushInput()
serialCom.setDTR(True)

print(formatting)
plane_data = {key: [] for key in formatting}

try:
    # reads line printed by arduino and converts it frombinary -> decoded string
    data_bytes = serialCom.readline()
    decoded_data= data_bytes.decode('utf-8').strip()
    print(decoded_data)

    # 

    values = decoded_data.strip().split(" ")

    plane_data = []

    if values and values[0] == key:
        for label, value in zip(formatting, values[1:]):
            plane_data[label].append(value)
    



except Exception as e:
    print(e)

