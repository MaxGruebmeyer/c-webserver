# C-Webserver

This repo contains a C-Webserver - not yet fully HTTP/1.1 compliant.
It's written using only the [C89 (ansi) standard libraries](https://en.wikibooks.org/wiki/C_Programming/Standard_libraries)
which means no `<sys/socket.h>`, no `<sys/types.h>` and also no `<unistd.h>`.

Well then, how did we do it?
We utilized some [Linux syscalls](https://thevivekpandey.github.io/posts/2017-09-25-linux-system-calls.html).
We put the syscall args into their respective registers before calling `__asm__("syscall")`.
The result is obviously hugely platform dependent since we use Linux kernel features that won't be available on
other operating systems. Furthermore even the registers (and also some syscall numbers) differ between 32 and 64 Bit Linux Systems,
so you can really only run it on a x86_64 Linux machine.

TODO (GM): Add an Ubuntu dockerfile so you can run the server on other operating systems as well.

## How to run

Since this project is so hugely platform dependent it doesn't make sense to build it for an e.g. windows or macOS
environment which is why the following only works on Linux systems. Note that we will supply a dev container later on
so we can support users on other operating systems as well.

TODO (GM): Add docker container!

You can utilize our project's [Taskfile](https://taskfile.dev/) to build and run both the server as well as a test client.
Use `task run` to run the server and `task run-test` to run the test client.

Of course you can also compile the project manually using gcc and linking all required files (which is all of
`src/*.c` for the server and all of `src/*.c` with `test/test-client.c` but exluding `src/main.c` for the test-client.

## TODO

- Allow serving of mutliple connections simultaneously (test this via Gatling or similar)
- Add Unit Tests
- Add actual logging library (write yourself for semesterproject compliance?)
- Support the most basic HTTP/1.1 Features
    - Basic request strucute
    - GET/POST/PUT/DEL
- Review what we can replace with the stdlibs!
- Support HTTPS
- Support keep-alive
- Provide different startup options via configuration file and other maybe during compile time
- Create build and run scripts (run only on unix since we use unistd.h)
- Create Dev container
- Provide basic CI incl. build and build instructions
- Create Docker image running the thing
- Deploy to Cloud
- Automize deployment via terraform (-> Maybe check how difficult it is to allow deployment to e.g. both AWS and GCP)
- Full HTTP/1.1 Compliance
- Full HTTP/2 Compliance
- Rewrite/extension for HTTP/3
- Do a security review
    - Watch out for buffer overflows
    - Do we need to support Auth of some sort?
- Use the webserver to host an actual website
- Perftest the shit out of it (& compare with other webservers)

## Writeup of dev process

- Try to do it with barebone tools and built in features
- Use man pages to see what we need to do (e.g. man socket)
- Use assembly to learn how to write a syscall (-> Register size is important!)
- When writing about the dev process go in steps, e.g. building something with all the libraries first and
then removing the libs one after the other until you're left with the C89 stdlibs only)
