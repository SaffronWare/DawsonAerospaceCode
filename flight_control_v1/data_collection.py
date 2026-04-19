import pyvista as pv
import serial
import time
from math import sin

plotter = pv.Plotter()

TESTING = True

plane    = pv.read("plane_model//Plane.stl")
rudder   = pv.read("plane_model//Rudder.stl")
aileronL = pv.read("plane_model//aileronL.stl")
aileronR = pv.read("plane_model//aileronR.stl")
elevator = pv.read("plane_model//Elevator.stl")

for mesh in [plane, rudder, aileronL, aileronR, elevator]:
    mesh.rotate_z(-90, inplace=True)

plane_actor    = plotter.add_mesh(plane)
rudder_actor   = plotter.add_mesh(rudder)
aileronL_actor = plotter.add_mesh(aileronL)
aileronR_actor = plotter.add_mesh(aileronR)
elevator_actor = plotter.add_mesh(elevator)

# Cache hinge centers BEFORE any animation (these are in neutral pose)
rudder_hinge   = rudder.center
aileronL_hinge = aileronL.center
aileronR_hinge = aileronR.center
elevator_hinge = elevator.center

plotter.show(auto_close=False, interactive_update=True)
plotter.camera_position = [
    (-40, 0, 20),
    (0, 0, 0),
    (0, 0, 1)
]
plotter.add_axes()

key = "DBG"
formatting = ["roll", "pitch", "yaw", "rudder", "aileron", "elevator"]


def apply_servo(actor, hinge_center, servo_axis, servo_angle_deg, roll, pitch, yaw):
    """
    Reset actor, rotate around its hinge by servo angle, then rotate with the plane.
    Order matters: servo deflection first (local space), then plane orientation on top.
    """
    actor.orientation = (0, 0, 0)

    # Step 1: servo deflection around hinge in neutral/local space
    actor.origin = hinge_center
    if servo_axis == 'x':
        actor.rotate_x(servo_angle_deg)
    elif servo_axis == 'y':
        actor.rotate_y(servo_angle_deg)
    elif servo_axis == 'z':
        actor.rotate_z(servo_angle_deg)
    actor.origin = (0, 0, 0)

    # Step 2: apply plane rotation so servo rides with the plane
    actor.rotate_z(-yaw)
    actor.rotate_y(-roll)
    actor.rotate_x(pitch)


with open("flight_data.csv", "w") as f:
    f.truncate()

    if not TESTING:
        serialCom = serial.serial_for_url('loop://', 115200, timeout=1)
        serialCom.setDTR(False)
        time.sleep(1)
        serialCom.flushInput()
        serialCom.setDTR(True)

    while plotter:
        try:
            if TESTING:
                data_bytes = (
                    f"DBG {45 * (sin(time.time()) + 1)/2} 0 0 "
                    f"{sin(time.time()) * 20} 10 {sin(4*time.time()) * 20}"
                ).encode()
                time.sleep(1/60)
            else:
                data_bytes = serialCom.readline()

            decoded_data = data_bytes.decode('utf-8').strip().split()

            if decoded_data[0] == key:
                decoded_data = [float(v) for v in decoded_data[1:]]

                roll          = decoded_data[formatting.index("roll")]
                pitch         = decoded_data[formatting.index("pitch")]
                yaw           = decoded_data[formatting.index("yaw")]
                rudderAngle   = decoded_data[formatting.index("rudder")]
                aileronAngle  = decoded_data[formatting.index("aileron")]
                elevatorAngle = decoded_data[formatting.index("elevator")]

                # Plane body
                plane_actor.orientation = (pitch, -roll, -yaw)

                # Servos: deflect around hinge, then rotate with plane
                apply_servo(rudder_actor,   rudder_hinge,   'z',  rudderAngle,   roll, pitch, yaw)
                apply_servo(elevator_actor, elevator_hinge, 'y',  elevatorAngle, roll, pitch, yaw)
                apply_servo(aileronL_actor, aileronL_hinge, 'y',  aileronAngle,  roll, pitch, yaw)
                apply_servo(aileronR_actor, aileronR_hinge, 'y', -aileronAngle,  roll, pitch, yaw)

                plotter.update()

        except Exception as e:
            print(f"error: {e}")