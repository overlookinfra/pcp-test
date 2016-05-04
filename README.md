# pcp-test

A framework for creating and executing performance tests on a PCP broker.

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

## Maintenance

Maintainers: Alessandro Parisi <alessandro@puppet.com>, Michael Smith
<michael.smith@puppet.com>, Michal Ruzicka <michal.ruzicka@puppet.com>.

Contributing: Please refer to [this](CONTRIBUTING.md) document.

Tickets: File bug tickets at https://tickets.puppet.com/browse/PCP and add the
`pcp-test` component to the ticket.
