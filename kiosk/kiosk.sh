#!/bin/sh
echo "Boot to SPL Kiosk: $(date)" > /etc/motd

sleep 10

DIR=${PWD}
cd /home/pete/working/SPL/kiosk
./env/bin/gunicorn --bind 0.0.0.0:5000 -w 1 --worker-class eventlet --log-file /var/log/spl.log \
--log-level 'debug' 'spl:create_app(8888)'
cd ${DIR}

sleep 1
DISPLAY=:0.0; export DISPLAY
/usr/bin/chromium-browser --display=${DISPLAY} --noerrdialogs --disable-infobars --kiosk --start-maximized http://127.0.0.1:5000 &

unclutter -idle 0.1 -display ${DISPLAY} -root &

