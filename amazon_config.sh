--
/usr/sbin/tunctl -t tap0 -u ec2-user
brctl addbr virbr0
brctl addif virbr0 eth1
brctl addif virbr0 tap0
ifconfig tap0 up
ifconfig virbr0 up
ifconfig virbr0 172.16.6.100 netmask 255.255.255.0
ifconfig eth1 0.0.0.0 promisc
sleep 3
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -F
iptables -t nat -A PREROUTING -p tcp --dport 21846 -j DNAT --to-destination 172.16.6.101:21846
iptables -t nat -A POSTROUTING --out-interface virbr0 -j MASQUERADE
#iptables -t nat -A POSTROUTING -p tcp -d 172.16.6.101 --dport 21846 -j SNAT --to-source 172.31.40.158
cd /home/ec2-user
losetup -o 1048576 /dev/loop5 ./disk.img
