## Connection Test

The objective of the Connection Test is to assess the number of PCP connections
a given PCP broker can handle.

### Configuration

The Connection Test establishes a number of PCP connections between a given PCP
broker (the first entry of the `broker-ws-uris` array; of course it must point
to an active PCP broker!) and a number of sets (given by the `concurrency`
option) of PCP clients (`num-endpoints`). If requested (by setting the the
`persist-connections` boolean option), connections are maintained open by
sending WebSocket pings.

Different sets of clients are connected in a concurrent way, whereas clients of
a given set are connected one at a time, with a given pause in between
(`inter-endpoint-pause-ms` in milliseconds).

The test is repeated a number of times (`num-runs`); each run may have the
number of sets or clients per set incremented (respectively, by
`concurrency-increment` and `endpoints-increment`). The test runner waits for a
given interval before starting a subsequent run (`inter-run-pause-ms` in
milliseconds).

WebSocket connections are established with a given timeout for the handshake
initialization (`ws-connection-timeout-ms` in milliseconds).

PCP Association requests are sent with a given TTL (`association-ttl-s` in
seconds). Note that such TTL is only meant for the processing of the request
message; instead, the timeout for the entire association process can be
specified by `association-timeout-s` (in seconds).

Note that the `broker-ws-uris` is a global option, whereas all other options
mentioned in this section are specific to the Connection Test and must be
specified in the configuration file, within the `connection-test-parameters`
entry (refer to the example [here](../README.md)). All options must be defined.
Also, all options have integer arguments. The complete list of options is:
 - `num-runs`
 - `inter-run-pause-ms`
 - `num-endpoints`
 - `inter-endpoint-pause-ms`
 - `concurrency`
 - `endpoints-increment`
 - `concurrency-increment`
 - `ws-connection-timeout-ms`
 - `association-timeout-s`
 - `association-request-ttl-s`
 - `persist-connections`

Once again, ALL options must be specified in your configuration file.

### Result Metrics

For each run, the number of total connections and failures is reported.
A connection is considered failed if a successful PCP Association response is
not received in 10 seconds.

More precisely, each row of the results CSV file (named
`connection_test_<date-time>.csv`) provides, in order, the number of clients
in each set, the number of sets, and the number of connection failures.

An example of output on standard out is the following:
```
    Connection test setup:
      4 concurrent sets (+0 per run) of 100 endpoints (+100 per run)
      5 runs, 30000 ms pause between each run
      50 ms pause between each endpoint connection
      WebSocket connection timeout 2500 ms
      Association timeout 10 s, Association Request TTL 5 s
      send WebSocket pings for keep-alive: yes

    Starting run 1: 4 concurrent sets of 100 endpoints
      [SUCCESS]  400 successful connections
    Starting run 2: 4 concurrent sets of 200 endpoints
      [SUCCESS]  800 successful connections
    Starting run 3: 4 concurrent sets of 300 endpoints
      [SUCCESS]  1200 successful connections
    Starting run 4: 4 concurrent sets of 400 endpoints
      [SUCCESS]  1600 successful connections
    Starting run 5: 4 concurrent sets of 500 endpoints
      [SUCCESS]  2000 successful connections

      Connection test: finished in 39 m 37 s
```
