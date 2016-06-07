# pcp-test

A framework for creating and executing performance tests on PCP brokers.

## Required packages

 - [cmake](https://cmake.org)
 - [Boost](http://boost.org)
 - [Leatherman](https://github.com/puppetlabs/leatherman)
 - [cpp-pcp-client](https://github.com/puppetlabs/cpp-pcp-client)

## Build pcp-test

Assuming the above packages installed, you simply:

```
    mkdir build && cd build
    cmake ..
    make -j
```

## SSL certificates

pcp-test requires SSL certificates signed by the same Certificate Authorithy
as the one used by the PCP brokers under test. You can use `puppet cert` for
that; please refer to the [pcp-broker docs](https://github.com/puppetlabs/pcp-broker/blob/master/doc/authentication.md).

The directory that contains the certificates can be specified by the
`--certificates-dir` option. The default directory is
`pcp-test/dev-resources/pcp-certificates`. The requirements for storing the
certificate files are explained in this [document](doc/certificates.md).

## Using pcp-test

`pcp-test` is invoked by command line as:
```
    pcp-test <test-type> global-options
```

`test-types` are:
 - `trivial`: a trivial test with 1 controller and 2 agents; no specific option is available for this test
 - `connection`: creates a number of PCP connections concurrently; more details [here](doc/connection.md)

`global-options` are:
```
      -h [ --help ]                  produce help message
      -v [ --version ]               print the version and exit
      --loglevel arg                 log level (none, trace, debug, info, warning, error, fatal)
      --logfile arg                  log file
      --config-file arg              configuration file (mandatory);
                                     defaults to /etc/puppetlabs/pcp-test/pcp-test.conf
      -u [ --broker-ws-uris ] arg    PCP broker WebSocket URIs
      --certificates-dir arg         SSL certificates path (see doc for the expected directory tree structure);
                                     defaults to <PCP_TEST_ROOT>/dev-resources/pcp-certificates
      --results-dir arg              results directory; defaults to /opt/puppetlabs/pcp-test/results
```

Other options are specific to each `test-type` and can be only specified in the
configuration file, in JSON format. Note that `global-options` can also be
specified in the configuration file, without the double hyphen prefix.

An example of configuration file is:
```
    {
        "logfile"  : "~/logs/pcp-test.log",
        "loglevel" : "trace",
        "broker-ws-uris" : ["localhost",
                            "broker.example.com"],
        "connection-test-parameters" : {
            "num-runs"                   : 1,
            "inter-run-pause-ms"         : 5000,
            "num-endpoints"              : 1,
            "inter-endpoint-pause-ms"    : 150,
            "concurrency"                : 1,
            "endpoints-increment"        : 0,
            "concurrency-increment"      : 0,
            "ws-connection-timeout-ms"   : 1000,
            "association-timeout-s"      : 5,
            "association-request-ttl-s"  : 10,
            "persist-connections"        : true
        }
    }
```

Results will be shown on standard out and written on a text file in CSV format.
Results files will be stored in the specified `results-dir` and named as
`<test-type>_<date-time>.csv`.

## Maintenance

Maintainers: Alessandro Parisi <alessandro@puppet.com>, Michael Smith
<michael.smith@puppet.com>, Michal Ruzicka <michal.ruzicka@puppet.com>.

Contributing: Please refer to [this](CONTRIBUTING.md) document.

Tickets: File bug tickets at https://tickets.puppet.com/browse/PCP and add the
`pcp-test` component to the ticket.
