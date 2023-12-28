./cmd.py "fibon 10" 1 & sleep 5 && ./cmd.py "crash" 1 &
sleep 0.5
./cmd.py "fibon 10" 2 & sleep 5 && ./cmd.py "crash" 2 &
sleep 0.5
./cmd.py "fibon 10" 3 & sleep 5 && ./cmd.py "crash" 3 &
sleep 0.5
./cmd.py "fibon 10" 4 & sleep 5 && ./cmd.py "crash" 4 &
sleep 0.5
./cmd.py "fibon 10" 5 & sleep 5 && ./cmd.py "crash" 5 &
sleep 0.5
./cmd.py "fibon 10" 6 & sleep 5 && ./cmd.py "crash" 6 &
sleep 0.5
./cmd.py "fibon 10" 7 & sleep 5 && ./cmd.py "crash" 7 &
sleep 0.5
./cmd.py "fibon 10" 8 & sleep 5 && ./cmd.py "crash" 8 &
sleep 0.5
./cmd.py "fibon 10" 9 & sleep 5 && ./cmd.py "crash" 9 &
sleep 0.5
./cmd.py "fibon 10" 10 & sleep 5 && ./cmd.py "crash" 10 &
date
