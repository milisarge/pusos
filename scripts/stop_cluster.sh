#1/bin/sh

count=$1

for n in $(seq 1 $count);do
  echo "pus$n kvm is killing"
  tmux kill-session -t "pus${n}"
  #sleep 1
done
