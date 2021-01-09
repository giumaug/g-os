losetup -o 1048576 /dev/loop7 /home/peppe/Scrivania/c.img
/usr/sbin/tunctl -t tap0 -u peppe
brctl addbr virbr0
brctl addif virbr0 enp1s0
brctl addif virbr0 tap0
ifconfig tap0 up
ifconfig virbr0 up
ifconfig virbr0 192.168.122.100 netmask 255.255.255.0
ifconfig  enp1s0 0.0.0.0 promisc
route add default gw 192.168.122.1 virbr0
