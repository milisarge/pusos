#1/bin/sh

#tmux new-session -d -s $1 "scripts/start_pus.sh $1"

count=$1

for n in $(seq 1 $count);do
  tmux new-session -d -s "pus${n}" "scripts/start_pus.sh $n"
  sleep 2
done
