for i in {1..50}; do echo "test$i" | ncat -u 192.168.100.18 777  ;done
