#!/bin/bash

if [ "$EUID" -ne 0 ]; then
  exec sudo "$0" "$@"
  exit $?
fi

if ! command -v efibootmgr &>/dev/null; then
    echo "Installing efibootmgr..."
    pacman -Sy --noconfirm efibootmgr
fi

current_boot=$(efibootmgr | awk '/BootCurrent/{print $2}')

echo "Current EFI Boot Entries:"
echo "-------------------------"
entries=()
i=0
while IFS= read -r line; do
    bootnum=$(echo "$line" | awk '{print $1}' | cut -c5-8)
    description=$(echo "$line" | cut -d' ' -f2-)
    
    [ "$bootnum" = "$current_boot" ] && description+=" (CURRENT)"
    
    entries+=("$bootnum")
    printf "%2d: %s\n" "$i" "$description"
    ((i++))
done < <(efibootmgr | grep '^Boot' | grep -v BootCurrent)

read -p "Enter numbers to delete (space-separated): " -a choices

for choice in "${choices[@]}"; do
    if (( choice < 0 || choice >= ${#entries[@]} )); then
        echo "Invalid choice: $choice"
        continue
    fi
    
    bootnum=${entries[choice]}
    
    if [ "$bootnum" = "$current_boot" ]; then
        echo "Skipping current boot entry: $bootnum"
        continue
    fi
    
    echo "Deleting Boot$bootnum..."
    efibootmgr -b "$bootnum" -B
done

echo -e "\nRemaining boot entries:"
efibootmgr
