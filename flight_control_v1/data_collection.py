import serial



ser = serial.Serial('COM3', 19200, timeout=1)

debug_mode = input("Debug mode (y/n): ").strip().lower() ==  "y"

def format_data(dbg_data):
    keys = ["ax", "ay", "az", "gx", "gy", "gz", "roll", "yaw", "pitch"]
    dict_data = {}
    for i,data_key in enumerate(keys):
        dict_data[data_key] = float(dbg_data[i+1])
    return dict_data


while True:
    line = ser.readline().decode().strip().split()

    if line:
        if line[0] == "DBG":
            if debug_mode:
                data = format_data(line)
                print(data)

        else:
            print(" ".join(line))
