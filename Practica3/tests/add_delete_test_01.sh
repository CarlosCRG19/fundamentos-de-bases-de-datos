#!/usr/bin/expect -f
# 05
# a) check that the book is deleted from index
## b) check that list of deleted books is created
##     and file with list is saved
## c) check add after delete with the different strategies
set timeout -1
set programName "library"
set filename "test"

# delete all files starting with test
# file delete [glob test*]
spawn rm -f $filename.db $filename.ind

# call program
#spawn valgrind ./$programName first_fit test
spawn ./$programName first_fit test
expect "Type command and argument/s."
expect "exit"

# ADD BOOKS
# add first book
send  "add 12346|978-2-12345680-3|El Quijote|Catedra\r"
expect "Record with BookID=12346 has been added to the database"
expect "exit"
# add second book
send  "add 12345|978-2-12345086-3|La busca|Catedra\r"
expect "Record with BookID=12345 has been added to the database"
expect "exit"

# add third book
send  "add 12347|978-2-12345680-4|el quijote|catedra\r"
expect "Record with BookID=12347 has been added to the database"
expect "exit"

# add fourth book
send  "add 12348|978-2-12345086-3|la busca|catedra\r"
expect "Record with BookID=12348 has been added to the database"
expect "exit"

# check index
# print index
puts "------------------------"
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #46"
expect "    size: #36"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #2"
expect "    key: #12347"
expect "    offset: #90"
expect "    size: #38"
expect "Entry #3"
expect "    key: #12348"
expect "    offset: #136"
expect "    size: #36"
expect "exit"

#delete book 12347
send "del 12347\r"
expect "Record with BookID=12347 has been deleted"
expect "exit"
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #46"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "Entry #2"
expect "    key: #12348"
expect "    offset: #136"
expect "exit"

#delete book 12345
send "del 12345\r"
expect "Record with BookID=12345 has been deleted"
expect "exit"
send "printInd\n"
expect "Entry #0"
expect "    key: #12346"
expect "    offset: #0"
expect "Entry #1"
expect "    key: #12348"
expect "    offset: #136"
expect "exit"

#delete book 12348 AGAIN
puts "Delete AGAIN 12345---------------------"
send "del 12345\r"
expect "Item with key 12345 does not exist"
expect "exit"
send "printInd\n"
expect "Entry #0"
expect "    key: #12346"
expect "    offset: #0"
expect "Entry #1"
expect "    key: #12348"
expect "    offset: #136"
expect "exit"
send "exit\n"
expect "all done"

puts  "1) Delete index records OK, ;-)"

if {[file exists [file join $filename.ind]]} {
    puts "2) file $filename.ind Exists, ;-)"
} else {
    puts "2) file $filename.ind NOT found, :-("
}

## call diff program
set output "differ"
try {
set output [exec diff -s $filename.ind ${filename}_control_del_01.ind]
} trap CHILDSTATUS {} {}
if {[regexp -nocase "identical" $output] || [regexp -nocase "idénticos" $output]} {
    puts "3) files are identical, ;-)"
} else {
    puts "3) files differ, :-("
}
puts "4) Script end"
