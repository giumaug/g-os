TO MAKE NETWORK WORKING PATCH QEMU:

RUN WITH
https://lists.gnu.org/archive/html/qemu-devel/2014-12/msg02818.html

diff --git a/hw/net/e1000.c b/hw/net/e1000.c
index ec9224b..82829ae 100644
--- a/hw/net/e1000.c
+++ b/hw/net/e1000.c
@@ -1544,8 +1544,15 @@ static void e1000_write_config(PCIDevice *pci_dev, 
uint32_t address,
 
     pci_default_write_config(pci_dev, address, val, len);
 
-    if (range_covers_byte(address, len, PCI_COMMAND) &&
-        (pci_dev->config[PCI_COMMAND] & PCI_COMMAND_MASTER)) {
+    if (range_covers_byte(address, len, PCI_COMMAND)) {
+        /*
+         * Some guest (eg: Win2012-64r2) doesn't enable bus mastering
+         * correctly, it caused guest network down. So we unconditionally
+         * enable PCI bus mastering and BM memory region for e1000 as
+         * a workaround.
+         */
+        pci_dev->config[PCI_COMMAND] |= PCI_COMMAND_MASTER;
+        memory_region_set_enabled(&pci_dev->bus_master_enable_region, true);
         qemu_flush_queued_packets(qemu_get_queue(s->nic));
         start_xmit(s);
     }


./qemu-system-i386 -s -S  -m 1000M -hda /home/peppe/Scrivania/build-g-os/c.img  -monitor stdio -device e1000,netdev=net0,mac=00:0C:29:10:64:B9 -netdev tap,id=net0,script=no,ifname=tap0
