#!/bin/bash

declare -A mounts

# I'm using this as an example you would change the mount directory and your devices LSBLK to view the drives you want I'm just using this for my movie server.
mounts["/dev/sdc"]="/mnt/movies1x"
mounts["/dev/sdg"]="/mnt/movies3x"
mounts["/dev/sdf"]="/mnt/movies2x"
mounts["/dev/sdi"]="/mnt/movies4x"

# Backup /etc/fstab
sudo cp /etc/fstab /etc/fstab.bak

for device in "${!mounts[@]}"; do
    uuid=$(blkid -s UUID -o value "$device")
    
    if [ -z "$uuid" ]; then
        echo "UUID not found for $device. Make sure the device is connected."
        continue
    fi
    mount_point="${mounts[$device]}"
    sudo mkdir -p "$mount_point"
    echo "UUID=$uuid $mount_point ext4 defaults 0 0" | sudo tee -a /etc/fstab
    echo "Added $device to /etc/fstab with UUID $uuid at $mount_point"
done

echo "Done. Please verify entries in /etc/fstab: a backup has been saved to /etc/fstab.bak incase something went wrong."
sudo tail -n 4 /etc/fstab
