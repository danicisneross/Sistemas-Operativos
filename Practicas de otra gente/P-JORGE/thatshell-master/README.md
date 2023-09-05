# thatshell
Operating Systems (CS Degree 2020/2021) lab assignment.  

## Avaliable commands
As this is an incremental assigment, the avaliable commands will be growing and separately listed.

### lab0 commands
```
authors [-l|-n]          Prints the names and logins of the program authors.
                    -l   Prints only the logins.
                    -n   Prints only the names.
        
getpid                   Prints the pid of the process executing the shell.
        
getppid                  Prints the pid of the shell's parent process.
        
pwd                      Prints the shell's current working directory.
        
chdir [direct]           Changes the current working directory of the shell to direct
                         (without arguments just prints the current working directory).
        
date                     Prints the current date in the format DD/MM/YYYY.
        
time                     Prints the current time in the format hh:mm:ss

historic [-c|-N|-rN]     Manages the historic of commands executed by this shell (without
                         arguments just prints the list of commands).
                    -c   Clears the entries on the historic list.
                    -N   Shows N first commands.
                    -rN  Repeats N command.

quit                     Ends the shell.

end                      Ends the shell.

exit                     Ends the shell.
```

## Working example
```
*) pwd
/home/antonio/Practicas
*) getppid
11562
*) getpid
11581
*) chdir /root
Cannot change dir /root: permission denied
*) chdir pru.c
Cannot change dir pru.c: not a directory
*) historic
0->pwd
1->getppid
2->getpid
3->chdir /root
4->chdir pru.c
6->historic
*) historic -2
0->pwd
1->getppid
2->getpid
*) historic -r3
chdir /root
Cannot change dir /root: permission denied
```