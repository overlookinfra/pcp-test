## Certificates Directory

The Certificates Directory is specified by the `--certificates-dir` option.
The default directory is `pcp-test/dev-resources/pcp-certificates`.

The Certificates Directory must contain the CA certificate that will be used to
authenticate PCP connections; such certificate file must be named `ca_crt.pem`.
Also, a subdirectory named `test` should store the pairs of SSL keys and signed
certificates that will be used to connect PCP agents and controllers. Lots of
them are required!
The names of those certificate files must have this format:

    `<COMMON NAME>.example.com_[crt|key].pem`

Where each common name has an integer prefix followed by a client type; the
format is `XXXX[agent|controller]`.

The contents of the Certificates Directory are:

    ├── ca_crt.pem
    └── test
        ├── 0000agent.example.com_crt.pem
        ├── 0000agent.example.com_key.pem
        ├── 0000controller.example.com_crt.pem
        ├── 0000controller.example.com_key.pem
        ├── 0001agent.example.com_crt.pem
        ...
        ├── 1999controller.example.com_key.pem
