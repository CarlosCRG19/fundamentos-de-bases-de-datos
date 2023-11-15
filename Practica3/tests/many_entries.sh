#!/usr/bin/expect -f
# introduce many books  Books (30000)
# execute  time ./many_entries.sh > /dev/null
# took 
# real	0m9,485s
# user	0m5,778s
# sys	0m4,249s


set timeout -1
set programName "library"
set filename "test"

# delete all files starting with test
# file delete [glob test*]
spawn rm -f $filename.db $filename.ind
expr {srand([clock seconds])}    ;# initialize RNG
spawn ./$programName best_fit test

for {set x 1001} {$x<30000} {incr x} {
   expect "exit"   
   puts "$x"
   send  "add $x|978-2-12345680-3|El Quijote|Catedra\r"
}
expect "exit"
send "printInd\n"
expect "exit"
send "exit\n"
expect "all done"
