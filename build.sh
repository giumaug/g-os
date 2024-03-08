BASE_DIR=/home/peppe/Desktop/g-os-grub2
target=$1;
if [[ $target == "a" ]] || [[ $target == "i" ]]    
then
   umount /dev/loop3
   umount /dev/loop4
   losetup -d /dev/loop3
   losetup -d /dev/loop4
#  cp ./c.img_bk_9_11_2019 c.img
#  cp ./c.img_with_dirs c.img
#  cp ./c.img_bk_03_08_2021 c.img attuale
   cp $BASE_DIR/g-os/img/gpt2.img_orig $BASE_DIR/g-os/img/c.img

   losetup -o 1048576 /dev/loop3 $BASE_DIR/g-os/img/c.img
   losetup -o 26214400 /dev/loop4 $BASE_DIR/g-os/img/c.img
   
   echo 'Image copy completed'
fi
if [[ $target == "a" ]] || [[ $target == "k" ]] 
then
   cd $BASE_DIR/g-os
   make clean
   make bochs
   echo 'Kernal build completed'
fi
if [[ $target == "a" ]] || [[ $target == "p" ]]    
then
   cd $BASE_DIR/g-os/process
   make process=shell clean
   make process=shell all
   make process=shell install
   make process=dev2 clean
   make process=dev2 all
   make process=dev2 install
   make process=dev3 clean
   make process=dev3 all
   make process=dev3 install
   make process=flush clean
   make process=flush all
   make process=flush install
#   make process=dev1 clean
#   make process=dev1 all
#   make process=dev1 install
#   make process=dev5 clean
#   make process=dev5 all
#   make process=dev5 install
   make process=dev6 clean
   make process=dev6 all
   make process=dev6 install
   make process=sp clean
   make process=sp all
   make process=sp install
#   make process=c1 clean
#   make process=c1 all
#   make process=c1 install
   make process=ht clean
   make process=ht all
   make process=ht install
#   make process=htc clean
#   make process=htc all
#   make process=htc install
#   make process=htcs clean
#   make process=htcs all
#   make process=htcs install
#   make process=del clean
#   make process=del all
#   make process=del install
    make process=file clean
    make process=file all
    make process=file install
    make process=ht clean
    make process=ht all
    make process=ht install
    make process=ss clean
    make process=ss all
    make process=ss install
    make process=udpc clean
    make process=udpc all
    make process=udpc install
    make process=screen clean
    make process=screen all
    make process=screen install
    make process=htc clean
    make process=htc all
    make process=htc install
    make process=htcs clean
    make process=htcs all
    make process=htcs install
    make process=test clean
    make process=test all
    make process=test install
    make process=open_conn clean
    make process=open_conn all
    make process=open_conn install

    mount /dev/loop4 /mnt
    echo $BASE_DIR
    cp $BASE_DIR/g-os/img/ht_files/8042.c /mnt
    cp $BASE_DIR/g-os/img/ht_files/tcp.c /mnt
    cp $BASE_DIR/g-os/img/ht_files/ext2.c /mnt
    cp $BASE_DIR/g-os/img/ht_files/vm.c /mnt
    cp $BASE_DIR/g-os/img/ht_files/scheduler.c /mnt
    umount /mnt
  echo 'Process build completed'
fi
