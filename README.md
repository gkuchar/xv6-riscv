# xv6 and Unix Utilities

## Background
xv6 is a reimplementation of the UNIX operating system in ANSI C for x86 and RISC-V systems.  It was created for educational purposes for MIT's Operating System Engineering course.

This lab will familiarize you with xv6 and its system calls.  

## Boot xv6 

You can do these labs on `babbage.cs.tcu.edu` or on your own computer (see me for the correct software packages). 

Go into your repo and retrieve any updates.  Make sure you have performed a

```text
git fetch
git pull
```

For this and subsequent assignments, any changes to the branch structure of your local git repo will need to also update your repo on GitHub Classroom.  

Type the following in your repo:

```text
git config --global push.autoSetupRemote true
```

For this assignment, you will create a new git branch: `utils`.  Type:

```text
git branch utils
git switch utils
```

This is a copy of the main branch and will allow to you make changes in this branch while keeping the original main version unchanged (for now).


### Build and Run xv6
At the prompt, type:

```text
make qemu
```

Should should see the following output:

```text
<LOTS OF COMPILING AND LINKING STATEMENTS>

qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 1 starting
hart 2 starting
init: starting sh
$
```

If you type `ls` at the prompt you should see the following:

```text
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2425
cat            2 3 35376
echo           2 4 34288
forktest       2 5 17232
grep           2 6 42728
init           2 7 34752
kill           2 8 34216
ln             2 9 34040
ls             2 10 41504
mkdir          2 11 34280
rm             2 12 34264
sh             2 13 56096
stressfs       2 14 35136
usertests      2 15 186024
grind          2 16 50488
wc             2 17 36320
zombie         2 18 33648
logstress      2 19 36184
forphan        2 20 35040
dorphan        2 21 34488
console        3 22 0
$
```

These are the files that `mkfs` includes in the initial file system; most are programs you can run. You just ran one of them: `ls`.
xv6 has no `ps` command, but, if you type `Ctrl-p`, the kernel will print information about each process. If you try it now, you'll see two lines: one for `init`, and one for `sh`.

To quit qemu type: `Ctrl-a x` (press `Ctrl` and `a` at the same time, followed by `x`).


## sleep (10 points)


> :memo: Implement a user-level `sleep` program for xv6, along the lines of the UNIX sleep command. Your `sleep` should pause for a user-specified number of ticks. A tick is a notion of time defined by the xv6 kernel, namely the time between two interrupts from the timer chip. Your solution should be in the file `user/sleep.c`.

Some hints:

+ Before you start coding, read Chapter 1 of the [xv6 book](https://pdos.csail.mit.edu/6.828/2023/xv6/book-riscv-rev3.pdf).
+ Put your code in `user/sleep.c`. Look at some of the other programs in `user/` (e.g., `user/echo.c`, `user/grep.c`, and `user/rm.c`) to see how command-line arguments are passed to a program.
+ Add your `sleep` program to `UPROGS` in `Makefile`; once you've done that, `make qemu` will compile your program and you'll be able to run it from the xv6 shell.
+ If the user forgets to pass an argument, `sleep` should print an error message.
+ The command-line argument is passed as a string; you can convert it to an integer using `atoi` (see `user/ulib.c`).
+ Use the system call `sleep`.
See `kernel/sysproc.c` for the xv6 kernel code that implements the `sleep` system call (look for `sys_sleep`), `user/user.h` for the C definition of sleep callable from a user program, and `user/usys.S` for the assembler code that jumps from user code into the kernel for sleep.
sleep's `main` should call `exit(0)` when it is done.


Run the program from the xv6 shell:
```text
...
init: starting sh
$ sleep 10
(nothing happens for a little while)
$
```


Your solution is correct if your program pauses when run as shown above.

I will test your sleep utility by executing:
```text
./grade-lab-util sleep
```

## pingpong (10 points)

> :memo: Write a user-level program that uses xv6 system calls to ''ping-pong'' a byte between two processes over a pair of pipes, one for each direction. The parent should send a byte to the child; the child should print "<pid>: received ping", where <pid> is its process ID, write the byte on the pipe to the parent, and exit; the parent should read the byte from the child, print "<pid>: received pong", and exit. Your solution should be in the file `user/pingpong.c`.

Some hints:
+ Add the program to `UPROGS` in Makefile.
+ Use `pipe` to create a pipe.
+ Use `fork` to create a child.
+ Use `read` to read from a pipe, and `write` to write to a pipe.
+ Use `getpid` to find the process ID of the calling process.
+ User programs on xv6 have a limited set of library functions available to them. You can see the list in `user/user.h`; the source (other than for system calls) is in `user/ulib.c`, `user/printf.c`, and `user/umalloc.c`.

Run the program from the xv6 shell and it should produce the following output:
```text
$ make qemu
...
init: starting sh
$ pingpong
4: received ping
3: received pong
$
```  
Your solution is correct if your program exchanges a byte between two processes and produces output as shown above.

I will test your pingpong utility by executing:
```text
./grade-lab-util pingpong
```


## primes (15 points)
> Write a concurrent version of prime sieve using pipes. This idea is due to Doug McIlroy, inventor of Unix pipes. The picture halfway down [this](https://swtch.com/~rsc/thread/) page and the surrounding text explain how to do it. Your solution should be in the file `user/primes.c`.

Your goal is to use `pipe` and `fork` to set up the pipeline. The first process feeds the numbers 2 through 35 into the pipeline. For each prime number, you will arrange to create one process that reads from its left neighbor over a pipe and writes to its right neighbor over another pipe. Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.

Some hints:
+ Be careful to close file descriptors that a process doesn't need, because otherwise your program will run xv6 out of resources before the first process reaches 35.
+ Once the first process reaches 35, it should wait until the entire pipeline terminates, including all children, grandchildren, &c. Thus the main primes process should only exit after all the output has been printed, and after all the other primes processes have exited.
+ Hint: read returns zero when the write-side of a pipe is closed.
+ It's simplest to directly write 32-bit (4-byte) ints to the pipes, rather than using formatted ASCII I/O.
+ You should create the processes in the pipeline only as they are needed.
+ Add the program to UPROGS in Makefile.
Your solution is correct if it implements a pipe-based sieve and produces the following output:

```text
$ make qemu
...
init: starting sh
$ primes
prime 2
prime 3
prime 5
prime 7
prime 11
prime 13
prime 17
prime 19
prime 23
prime 29
prime 31
$
```  

I will test your primes utility by executing:
```text
./grade-lab-util primes
```

