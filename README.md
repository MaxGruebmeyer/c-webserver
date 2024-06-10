# C-Webserver

This repo contains a C-Webserver - not yet fully HTTP/1.1 compliant.
It's written using only the [C89 (ansi) standard libraries](https://en.wikibooks.org/wiki/C_Programming/Standard_libraries)
which means no `<sys/socket.h>`, no `<sys/types.h>` and also no `<unistd.h>`.

Well then, how did we do it?
We utilized some [Linux syscalls](https://thevivekpandey.github.io/posts/2017-09-25-linux-system-calls.html) - mainly fork and the socket API -
to provide the required functionality by placing the required args into their respective registers before calling `__asm__("syscall")`.
The result is obviously hugely platform dependent since we use Linux kernel features that won't be available on
other operating systems. Furthermore you have to keep in mind the registers (and also some syscall numbers) differ between 32 and 64 Bit Linux Systems,
so you can really only run it on a x86_64 Linux machine.

However not everyone is running x86_64 Linux, so we created an Ubuntu [docker container](https://www.docker.com/resources/what-container/) that you can
use to run the server. Scroll down below to find out how it works.

## How to run

### Some remarks about supported operating systems

Since this project is so hugely platform dependent it doesn't make sense to build it for an e.g. Windows or MacOS
environment which is why the following only works on Linux x86_64 systems. However, we provide an Ubuntu dev container that you can
leverage to simulate a Linux system and run our server.

Note that this **MIGHT** work, but because of huge platform dependence this is not at all a given!

### Well, how do I run the thing on Linux x86_64?

You can utilize our project's [Taskfile](https://taskfile.dev/) to build and run both the server via `task run`.
Then you can just use a test client of your choice, e.g. browser or curl to send requests to the server on `localhost:8080`.

Of course you can also compile the project manually using gcc and linking all required files (e.g. everything in `src`).

### I'm on x86_64 but not on Linux, what do I do?

For people not using x86_64 Linux we developed an Ubuntu [docker container](https://www.docker.com/resources/what-container/) that you
can use to run the server. If you're unfamiliar with Docker you can learn how to get started [here](https://docs.docker.com/get-started/) or learn
how to install it [here](https://docs.docker.com/engine/install/).

The recommended way to start the container is via our [docker-compose](https://docs.docker.com/compose/) file.
This file does two things:

- It mounts this directory at `/opt/c-webserver` inside the docker container so code changes made on the host system are reflected inside the container.
- It forwards localhost:8080 to port 8080 on the container so you can access the webserver via the browser on your host system.

Use `docker compose up -d` to start the container in detached mode then use `docker ps --all` to list all containers.
Get the container id and connect to the container via `docker exec -it $ID /bin/bash`.
Then navigate to the sourcecode at `/opt/c-webserver` and use `task run` to start the server.
You should now be able to access the webserver at `localhost:8080` on your host system (because of the port forwarding rule).
If you make any code changes on your host system these are automatically reflected inside the container - just restart the server and they should be live.

### I'm not on x86_64 but instead on [i386, ARM, ...] can I still run the server?

Sadly this is a scenario that we do not support, since we have not tested it yet.
However, docker supports emulation so not all hope is lost. [The docker multi-platform guide](https://docs.docker.com/build/guide/multi-platform/)
might be a good starting point.

If you can emulate x86_64 architecture in your docker container you can probably just run the dev container and everything should be fine.
If you decide to go down that route, please feel free to contact us, we'll gladly provide support.

## TODO

- Port the whole thing to C11
- Get rid of the platform dependence and use some libraries - like <netinet.h> and <unistd.h>
- Get a domain and host the thing on AWS
- Implement TCP state machine so connection terminates correctly (so we avoid TIME_WAIT)
- Introduce unit tests using `<assert.h>` (instead of Unity - also applies to Minesweeper.h -> Do we need Mocking? If so, how can we implement it?
- Clean up code, structure it nicely etc.
- Make port configurable via config file and not during compile time!
- Clean up `src` folder, e.g. put header files in a subfolder, group files together etc
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
- Provide basic CI incl. build and build instructions
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
- Use assembly to learn how to write a syscall (-> Register size is important!) -> NULL args are also important
- When writing about the dev process go in steps, e.g. building something with all the libraries first and
then removing the libs one after the other until you're left with the C89 stdlibs only)
- "Multithreading" via fork
- How 2 routing -> Array because not a lot of routes -> Performance + Simplicity -> Maybe BST or HashMap later
