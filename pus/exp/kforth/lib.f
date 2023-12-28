: count 0 swap 1 for A . forloop ;
: rect 0 10 1 for 0 5 1 for "*" forloop cr forloop ;
: is0 0 = if "Yep, it's 0" cr elif "Nope, not 0" cr endif "Done" cr ;

: star "*" ;
: stars 0 do star loop ;
: square dup 0 do dup stars cr loop drop ;
: triangle 1 + 1 do I stars cr loop ;
: tower dup 1 - triangle square ;

: fib_rec dup 2 < if elif 1 - dup fib_rec swap 1 - fib_rec + endif ;
: fib fib_rec . ;
: time_fib time swap fib time swap - . "seconds to compute" cr ;
