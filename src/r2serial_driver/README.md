# Ros_rs485_interface
# package giao tiếp với driver BLDC MD200

- Connect module USB -> RS485
- Check serial port: $ls /dev/ttyUSB*
- Allows access: $sudo chmod 777 /dev/ttyUSBx
- $ roslaunch r2serial_driver r2serial_driver.launch port:=/dev/ttyUSBx baud:= 19200


