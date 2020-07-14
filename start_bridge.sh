losetup -o 1048576 /dev/loop7 /opt/virtutech/simics-3.0.31/workspace/c.img
/usr/sbin/tunctl -t tap0 -u peppe
brctl addbr virbr0
brctl addif virbr0 eth4
brctl addif virbr0 tap0
ifconfig tap0 up
ifconfig virbr0 up
ifconfig virbr0 192.168.128.100 netmask 255.255.255.0
ifconfig eth4 0.0.0.0 promisc
route add default gw 192.168.128.2 virbr0
