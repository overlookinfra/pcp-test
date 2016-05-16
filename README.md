# pcp-test

A framework for creating and executing performance tests on PCP brokers.

## Required packages

 - [cmake](https://cmake.org)
 - [Boost](http://boost.org)
 - [Leatherman](https://github.com/puppetlabs/leatherman)
 - [cpp-pcp-client](https://github.com/puppetlabs/cpp-pcp-client)

## Build pcp-test

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

## Maintenance

Maintainers: Alessandro Parisi <alessandro@puppet.com>, Michael Smith
<michael.smith@puppet.com>, Michal Ruzicka <michal.ruzicka@puppet.com>.

Contributing: Please refer to [this](CONTRIBUTING.md) document.

Tickets: File bug tickets at https://tickets.puppet.com/browse/PCP and add the
`pcp-test` component to the ticket.
