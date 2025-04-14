#!/bin/bash

echo "Downloading yum-utils, createrepo and httpd"
dnf install -y yum-utils createrepo httpd
printf "\n"

FILE_PATH="/var/tmp/ukol.img"
LOOP_DEVICE="/dev/loop0"
MOUNT_PATH="/var/www/html/ukol"

echo "Creating the 200MB file..."
dd if=/dev/zero of=$FILE_PATH bs=1MB count=200
printf "\n"
echo "Creating loop device..."
LOOP_DEVICE=$(losetup --show --find $FILE_PATH)
printf "\n"
echo "Creating filesystem on the new loop device..."
mkfs.ext4 $LOOP_DEVICE
printf "\n"

echo "Appending the filesystem info to /etc/fstab..."
UUID_LD=$(blkid -s UUID -o value $LOOP_DEVICE)

mkdir -p $MOUNT_PATH

echo "UUID=$UUID_LD $MOUNT_PATH ext4 loop 0 2" | tee -a /etc/fstab
printf "\n"

echo "Running daemon-reload..."
systemctl daemon-reload
printf "\n"
echo "Mounting..."
mount -a
printf "\n"

echo "Checking if it was successful..."
df -h | grep $MOUNT_PATH
printf "\n"

echo "Downloading the packages given as arguments to the $MOUNT_PATH..."
if [ $# -ne 0 ]; then
    yumdownloader --destdir=$MOUNT_PATH $@
else
    echo "Nothing to download..."
fi
printf "\n"

echo "Generating repodata..."
createrepo /var/www/html/ukol
printf "\n"
echo "Running 'restorecon -Rv /var/www/html/ukol'..."
restorecon -Rv /var/www/html/ukol
printf "\n"


echo "Configurating /etc/yum.repos.d/ukol.repo..."
touch /etc/yum.repos.d/ukol.repo

printf "[ukol]\nname=Ukol\nbaseurl=http://localhost/ukol\nenabled=1\ngpgcheck=0" | tee -a /etc/yum.repos.d/ukol.repo
printf "\n"
printf "\n"

echo "Starting and enabling httpd service..."
systemctl start httpd
systemctl enable httpd
printf "\n"


echo "Listing available yum repositories..."
yum repolist
printf "\n"

echo "Unmounting the filesystem attached to $MOUNT_PATH..."
umount /var/www/html/ukol
printf "\n"
echo "Running 'df -h | grep $MOUNT_PATH' to make sure..."
df -h | grep $MOUNT_PATH
printf "\n"

echo "Mounting..."
mount -a
printf "\n"
echo "Checking if it was successful by running command - findmnt $MOUNT_PATH - ..."
findmnt $MOUNT_PATH
printf "\n"

echo "Listing available packages..."
yum --disablerepo="*" --enablerepo="ukol" info available
