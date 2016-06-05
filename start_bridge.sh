/usr/sbin/tunctl -t tap0 -u peppe
brctl addbr virbr0
brctl addif virbr0 enp2s0
brctl addif virbr0 tap0
ifconfig tap0 up
ifconfig virbr0 up
ifconfig virbr0 172.16.6.100 netmask 255.255.255.0
ifconfig enp2s0 0.0.0.0 promisc
route add default gw 172.16.243.2 virbr0
