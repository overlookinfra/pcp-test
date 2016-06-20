## Connection Test

The objective of the Connection Test is to assess the number of PCP connections
a given PCP broker can handle.

### Configuration

The Connection Test establishes a number of PCP connections between a given PCP
broker (the first entry of the `broker-ws-uris` array; of course it must point
to an active PCP broker!) and multiple PCP clients created by pcp-test itself.

The PCP clients are grouped in a number of sets, specified by the `concurrency`
option; each set will contain `num-endpoints` clients. If requested, by setting
the `persist-connections` boolean option, connections are maintained open by
sending WebSocket pings with a given frequency; the ping period is given by the
`ws-connection-check-interval-s` option, in seconds.

Different sets of clients are connected in a concurrent way, whereas clients of
a given set are connected one at a time, with a given pause in between
(`inter-endpoint-pause-ms` in milliseconds).

The test is repeated a number of times (`num-runs`); each run may have the
number of sets or clients per set incremented (respectively, by
`concurrency-increment` and `endpoints-increment`). The test runner waits for a
given interval before starting a subsequent run (`inter-run-pause-ms`, in
milliseconds, plus 50 ms for each established connection).

WebSocket connections are established with a given timeout for the handshake
initialization (`ws-connection-timeout-ms` in milliseconds).

PCP Association requests are sent with a given TTL (`association-ttl-s` in
seconds). Note that such TTL is only meant for the processing of the request
message; instead, the timeout for the entire association process can be
specified by `association-timeout-s` (in seconds).

Note that the `broker-ws-uris` is a global option, whereas all other options
mentioned in this section are specific to the Connection Test and should be
specified in the JSON configuration file in the `connection-test-parameters`
object (refer to the example below).

The following are the mandatory options:

| name | type
|------|-----
|  `num-runs` | integer
|  `inter-run-pause-ms` | integer
|  `num-endpoints` | integer
|  `inter-endpoint-pause-ms` | integer
|  `concurrency` | integer
|  `endpoints-increment` | integer
|  `concurrency-increment` | integer

The following are non-mandatory options, with related default values:

| name | type | default value
|------|------|-----------|--------------
|  `ws-connection-timeout-ms` | integer | 1500 ms
|  `ws-connection-check-interval-s` | integer | 15 s
|  `association-timeout-s` | integer | 10 s
|  `association-request-ttl-s` | integer | 5 s
|  `persist-connections` | bool | `false`

Once again, ALL mandatory options must be specified in your configuration file,
in the `connection-test-parameter` object.

### Run Success

A given run is considered successful if all PCP connections are correctly
established within the time interval given by the following formula (in seconds):
```
    num-endpoints * (MAX(1, CEIL(inter-endpoint-pause-ms / 1000)) + association-timeout-s)
```

### Result Metrics

For each run, the number of total connections and failures is reported.
More precisely, each row of the results CSV file (named
`connection_test_<date-time>.csv`) provides, in order, the number of clients
in each set, the number of sets, and the number of connection failures.

An example of output on standard out is:
```
   ~/pcp-test/build/bin ❯ ./pcp-test connection

   Connection test setup:
     20 concurrent sets (+0 per run) of 12 endpoints (+1 per run)
     4 runs, (100 + 50 * num_endpoints) ms pause between each run
     2000 ms pause between each endpoint connection
     WebSocket connection timeout 2000 ms
     Association timeout 14 s; Association Request TTL 5 s
     keep WebSocket connections alive: yes, by pinging every 2 s

   Starting run 1: 20 concurrent sets of 12 endpoints must be established in 192 s
     [SUCCESS]  240 successful connections
   Starting run 2: 20 concurrent sets of 13 endpoints must be established in 208 s
     [SUCCESS]  260 successful connections
   Starting run 3: 20 concurrent sets of 14 endpoints must be established in 224 s
     [SUCCESS]  280 successful connections
   Starting run 4: 20 concurrent sets of 15 endpoints must be established in 240 s
     [SUCCESS]  300 successful connections

     Connection test: finished in 2 m 59 s
```

The related configuration of the `pcp-test.conf` file for the above test is:
```
    {
        "logfile"         : "~/logs/pcp-test.log",
        "loglevel"        : "info",
        "client-loglevel" : "trace",
        "broker-ws-uris"  : ["wss://broker.example.com:8142/pcp"],
        "connection-test-parameters" : {
            "num-runs"                       : 4,
            "inter-run-pause-ms"             : 100,
            "num-endpoints"                  : 12,
            "inter-endpoint-pause-ms"        : 2000,
            "concurrency"                    : 20,
            "endpoints-increment"            : 1,
            "concurrency-increment"          : 0,
            "ws-connection-timeout-ms"       : 2000,
            "ws-connection-check-interval-s" : 2,
            "association-timeout-s"          : 14,
            "association-request-ttl-s"      : 5,
            "persist-connections"            : true
        }
    }
```