# xv6 Unix Utilities

A collection of low-level systems programs written in C for the **xv6 operating system**: a minimal UNIX reimplementation targeting RISC-V architecture, used in MIT's OS Engineering course.

This project demonstrates hands-on experience with **operating system internals**, **inter-process communication**, and **concurrent pipeline design** using raw POSIX-style system calls. There is no standard library abstractions.

---

## Programs

### `primes` — Concurrent Prime Sieve via Process Pipeline
**Concepts:** `fork`, `pipe`, `IPC`, concurrent process trees, resource management

Implements Doug McIlroy's classic concurrent prime sieve using a dynamically spawned chain of processes connected by pipes. Each process in the pipeline:
- Reads integers from its left neighbor
- Identifies the first as a prime and prints it
- Filters out all multiples and forwards the rest to a newly spawned right neighbor

The result is a self-assembling pipeline of processes that cooperatively computes all primes up to 35. Special care was taken to close unused file descriptors at every stage to avoid exhausting xv6's limited fd table, and `wait(0)` is used to ensure proper tear-down of the entire process tree before exit.

```
$ primes
prime 2
prime 3
prime 5
...
prime 31
```

---

### `pingpong` — Bidirectional Byte Exchange Between Processes
**Concepts:** `fork`, dual `pipe`, synchronization, process coordination

Creates two unidirectional pipes to pass a single byte between a parent and child process. The parent writes to the child, the child receives and prints "received ping", then writes back; the parent reads and prints "received pong". Demonstrates careful pipe endpoint management: closing unused read/write ends to prevent deadlocks and resource leaks.

```
$ pingpong
4: received ping
3: received pong
```

---

### `sleep` — Kernel Tick-Based Pause
**Concepts:** system call interface, argument validation, user-space C

Implements the `sleep` system call wrapper with input validation, including a custom digit-check function to reject non-numeric arguments before passing ticks to the kernel. Clean error messaging and correct `exit` codes throughout.

```
$ sleep 10
(pauses for 10 kernel ticks)
```

---

## Technical Context

| Detail | Value |
|---|---|
| Language | C (ANSI) |
| Target OS | xv6 (RISC-V) |
| System Calls Used | `fork`, `pipe`, `read`, `write`, `wait`, `exit`, `getpid`, `sleep` |
| Build System | GNU Make + QEMU |
| Course Context | Operating Systems (TCU CS) |

---

## Skills Demonstrated

- **Systems programming in C** with no standard library safety net
- **IPC via pipes** — including bidirectional and chained pipeline patterns
- **Process lifecycle management** — `fork`/`wait` hierarchies, preventing zombie processes
- **File descriptor discipline** — explicitly closing all unused ends to avoid deadlocks and fd exhaustion
- **Concurrent design** — recursive process spawning synchronized through pipe I/O

---

## Running Locally

```bash
# Requires QEMU and a RISC-V toolchain
make qemu

# Inside xv6 shell:
$ sleep 10
$ pingpong
$ primes
```

---

**Author**: Griffin Kuchar  
**Course**: Operating Systems  
**Institution**: Texas Christian University  
**Date**: March 2026
