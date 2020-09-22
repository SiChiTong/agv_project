rosclean purge
source ./devel/setup.bash
sudo ip link set can0 up type can bitrate 125000
ip -details link show can0 
sudo chmod	666 /dev/ttyUSB*
source ./devel/setup.bash
roslaunch -v --screen sick_line_guidance sick_line_guidance.launch yaml:=sick_line_guidance_mls.yaml


