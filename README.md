# C-Webserver

## TODO

- Open TCP Socket and make basic connection
- Allow serving of mutliple connections simultaneously (test this via Gatling or similar)
- Support the most basic HTTP/1.1 Features
    - Basic request strucute
    - GET/POST/PUT/DEL
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
