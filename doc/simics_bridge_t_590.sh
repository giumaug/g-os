
#HOST

#systemctl stop firewalld
#iptables -L -n -v

#/usr/sbin/tunctl -t tap0 -u peppe
ip tuntap add dev tap0 mode tap user peppe
ip link add enp0s31f6 type dummy
brctl addbr virbr0
brctl addif virbr0 enp0s31f6
sudo brctl addif virbr0 tap0

ifconfig enp0s31f6 up
ifconfig tap0 up
ifconfig virbr0 up

ifconfig virbr0 192.168.5.60/24
ifconfig enp0s31f6 0.0.0.0 promisc

sysctl -w net.ipv4.ip_forward=1

#iptables -A FORWARD -i virbr0 -o enp0s31f6 -j ACCEPT
#iptables -t nat -A POSTROUTING -o enp0s31f6 -j MASQUERADE 
#iptables -A FORWARD -i enp0s31f6 -o virbr0 -m state --state RELATED,ESTABLISHED -j ACCEPT

#WIFI
#iptables -A FORWARD -i virbr0 -o wlp0s20f3 -j ACCEPT
#iptables -t nat -A POSTROUTING -o wlp0s20f3 -j MASQUERADE 
#iptables -A FORWARD -i wlp0s20f3 -o virbr0 -m state --state RELATED,ESTABLISHED -j ACCEPT 

#GUEST
#ifconfig enp0s25 192.168.5.65 netmask 255.255.255.0 up
##route add -net 192.168.1.0 netmask 255.255.255.0 dev enp0s25
#route add default gw 192.168.5.60 enp0s25
#tcpdump -U -XX -vv -e -nn -i wlp0s20f3 -i virbr0








