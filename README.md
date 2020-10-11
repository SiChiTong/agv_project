# catkin_ws


#Khởi tạo MLSE:
- Tạo shell cmd moi,tao file duoi .sh,copy dong lenh duoi:
	- rosclean purge
	- source ./devel/setup.bash
	- sudo ip link set can0 up type can bitrate 125000
	- ip -details link show can0 
	- source ./devel/setup.bash
	- roslaunch -v --screen sick_line_guidance sick_line_guidance.launch yaml:=sick_line_guidance_mls.yaml

#KHởi tạo USB->rs485:

Bước 1: Cấu hình USB->rs485:
	- Mở cổng terminal, Gõ lệnh "sudo touch /etc/udev/rules.d/99-usb-serial.rules" 
	- Mở file rules "sudo nano /etc/udev/rules.d/99-usb-serial.rules"
	- Copy 2 dòng lệnh sau: 
		- KERNEL=="ttyUSB[0-9]*", OWNER="robotics"
		- SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{manufacturer}=="PRATI", ATTRS{product}=="AGV-DRIVER-BLDV20KM", SYMLINK+="AGV-BLDV20KM"
	- Save và exit
	- kiểm tra thuộc tính "udevadm info --name=/dev/ttyUSB0 --attribute-walk"
- Nếu đã cấu hình rồi thì bỏ qua bước 1, IPC sẽ tự động thực hiện và cấp quyền cho cổng serial mỗi khi khởi động. Đồng thời, Không cần dùng lệnh  "sudo chmod 666 /dev/ttyUSB*".

Bước 2:
	- Kiểm tra USB->rs485 đã kết nối ok chưa. Gõ lệnh "ls /dev". Bạn sẽ thấy thiết bị tên "AGV-BLDV20KM". Nếu không có thì chưa kết nối được
	- KHởi chạy chương trình "roslaunch driver_blvd_controller agvrun.launch"
