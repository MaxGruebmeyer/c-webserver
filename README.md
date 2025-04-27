# C-Webserver

This repo contains a C-Webserver - not yet fully HTTP/1.1 compliant.
Since C is a pretty barebone language it utilizes a lot of Unix syscalls to achieve
the desired funtionality. This of course includes the Sockets API, but also
fork syscalls to support multithreading.

The webserver currently supports following functionality:
- Handles basic HTTP GET connections
- Support basic routing
- Returns hardcoded HTML
- Supports multiple simultaneous connections (via fork)

While the following features are still under construction:
- Error page currently returns 200 OK instead of 404 NOT FOUND.
- Handle more than just GET Requests
- Allow for query parameters and html bodies
- Returns .html files instead of hardcoding HTML in a .c file
- ... and much more

Currently the routes '/Unix' and '/Matheraum' are defined, as well as the index page.
Everything else should lead to an HTTP 404 Error Page.

## How to run

### Some remarks about supported operating systems

Since this project hugely utilizes the Unix syscalls it will only run on Unix systems.
However, since I've only tested it on Linux x86_64 and don't want to make any guarantees about non-Linux systems.

### Well, how do I run the thing on Linux?

You can utilize our project's [Taskfile](https://taskfile.dev/) to build and run both the server via `task run`.
Then you can just use a test client of your choice, e.g. browser or curl to send requests to the server on `localhost:8080`.

Of course you can also compile the project manually using a compiler of your choice
(gcc, llvm, ...) and linking all required files (e.g. everything in `src`).

### I'm not on Linux, what do I do?

For people not using Linux we developed an Ubuntu [docker container](https://www.docker.com/resources/what-container/) that you
can use to run the server. If you're unfamiliar with Docker you can learn how to get started [here](https://docs.docker.com/get-started/) or learn
how to install it [here](https://docs.docker.com/engine/install/).

The recommended way to start the container is via our [docker-compose](https://docs.docker.com/compose/) file.
This file does two things:

- It mounts this directory at `/opt/c-webserver` inside the docker container so code changes made on the host system are reflected inside the container.
- It forwards localhost:8080 to port 8080 on the container so you can access the webserver via the browser on your host system.

Per default, the container starts the server and exits once the server exits.
However, you can change this behaviour for debugging purposes.

To do this simply change the `ENTRYPOINT` in the `DOCKERFILE`. Don't worry, it has two entrypoints,
one active and one commented out. Just comment out the active one and activate the one commented out.
Then use `docker compose up -d` to start the container in detached mode then use `docker ps --all` to list all containers.
Get the container id and connect to the container via `docker exec -it $ID /bin/bash` and use `task run` to start the server.
You should now be able to access the webserver at `localhost:8080` on your host system (because of the port forwarding rule).
If you make any code changes on your host system these are automatically reflected inside the container - just restart the server and they should be live.

## TODO

- Implement document generation with doxygen
- Analyze the program for memory leaks with valgrind
- Implement TCP state machine so connection terminates correctly (so we avoid TIME_WAIT)
- Implement unit tests - possibly with Unity
- Add static + dynamic asserts where it makes sense
- Make port configurable via config file and not during compile time!
- Clean up `src` folder, e.g. put header files in a subfolder, group files together etc
- Allow serving of mutliple connections simultaneously (test this via Gatling or similar)
- Support the most basic HTTP/1.1 Features
    - Basic request structure
    - GET/POST/PUT/DEL
- Support HTTPS
- Support keep-alive
- Provide different startup options via configuration file and other maybe during compile time
- Provide basic CI incl. build and build instructions
- Get a domain and host the thing on AWS
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
- Switch to C11 and standard libraries, e.g. <unistd.h>, <netinet/in.h>, ...
