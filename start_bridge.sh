losetup -o 1048576 /dev/loop7 /opt/virtutech/simics-3.0.31/workspace/c.img
/usr/sbin/tunctl -t tap0 -u peppe
brctl addbr virbr0
brctl addif virbr0 eth3
brctl addif virbr0 tap0
ifconfig tap0 up
ifconfig virbr0 up
ifconfig virbr0 192.168.124.100 netmask 255.255.255.0
ifconfig eth3 0.0.0.0 promisc
route add default gw 192.168.124.2 virbr0
