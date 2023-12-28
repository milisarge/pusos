: fib_rec dup 2 < if elif 1 - dup fib_rec swap 1 - fib_rec + endif ;
: fib fib_rec . ;
: time_fib time swap fib time swap - . "seconds to compute" cr ;
