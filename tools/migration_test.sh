date
./cmd.py "fibon 10" 1 &
./cmd.py "fibon 10" 2 &
sleep 4 && ./recovery.py migrate "192.168.122.189" 4511 &
date
sleep 4 && ./recovery.py migrate "192.168.122.190" 4512 &
date
