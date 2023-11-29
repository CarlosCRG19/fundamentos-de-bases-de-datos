#!/usr/bin/expect -f
# 04
# add test no check if index is loaded
# properly when program is started
set timeout -1
set programName "library"
set filename "test"

# create index and database
exec "./add_index_test.sh"

#restart program
spawn ./$programName first_fit test

# print index
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
send "exit\n"
expect "all done"
puts "1) Index creating OK, ;-)"
puts "2) Script end"
