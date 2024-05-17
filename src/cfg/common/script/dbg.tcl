# connect
proc cn { } {
    connect -host localhost -port 3121
    targets -set -filter {name =~ "ARM* #0"}
}

# reload
proc rl { } {
    rst
    dow build/zed/sw/apu/bld/bld.elf
    con 
}

# restart
proc rs { } {
    rl
    con 
}

