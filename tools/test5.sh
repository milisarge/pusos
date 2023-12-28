./cmd.py "fibon 10" 1 & sleep 5 && ./cmd.py "crash" 1 &
./cmd.py "fibon 10" 3 & sleep 5 && ./cmd.py "crash" 3 &
./cmd.py "fibon 10" 5 & sleep 5 && ./cmd.py "crash" 5 &
./cmd.py "fibon 10" 7 & sleep 5 && ./cmd.py "crash" 7 &
./cmd.py "fibon 10" 9 & sleep 5 && ./cmd.py "crash" 9 &
date
