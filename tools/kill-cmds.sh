ps -aux | grep "python3 ./cmd.py" | awk '{print $2}' | xargs -I {} kill -9 {}
