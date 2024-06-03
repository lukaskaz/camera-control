#!/bin/bash

BUSID=1-3

if [ -z "$1" ]; then
    sudo insmod /lib/modules/5.15.146.1-microsoft-standard-WSL2/kernel/drivers/usb/serial/cp210x.ko
    sudo insmod /lib/modules/5.15.146.1-microsoft-standard-WSL2/kernel/drivers/usb/serial/usbserial.ko
    usbipd.exe attach --wsl -b $BUSID
    sleep 1
    usbipd.exe list

    ls /dev/ | grep -i "ttyusb"
    if [ $? -ne 0 ]; then
        echo "USB device not attacched, repeating"
        usbipd.exe attach --wsl -b $BUSID
        sleep 1
        usbipd.exe list
        ls /dev/ | grep -i "ttyusb"
    fi
else
    usbipd.exe detach -b $BUSID
fi

exit 0

