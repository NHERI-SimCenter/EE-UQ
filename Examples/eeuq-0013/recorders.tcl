set pid [getPID]
if {$pid == 0} {
    # this is recorder file
    file mkdir results.out
    set f [open results.out/disp.out "w"]
    puts $f "0.2"
    close $f
}
exit