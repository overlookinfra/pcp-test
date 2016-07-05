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
a given set are connected one at a time, with a constant pause in between
(`inter-endpoint-pause-ms` in milliseconds) in case the randomize flag is unset
(`randomize-inter-endpoint-pause`, defaults to `false`), otherwise pauses will
be obtained from an RNG that follows an exponential probability distribution
(mean rate given by the inverse of the specified pause) with a given seed
(`inter-endpoint-pause-rng-seed`, defaults to 1).

The test is repeated a number of times (`num-runs`); each run may have the
number of sets or clients per set incremented (respectively, by
`concurrency-increment` and `endpoints-increment`). The test runner waits for a
given interval before starting a subsequent run: a fixed interval of 2 s plus
`inter-run-pause-ms`, in milliseconds, for each established connection. Note
that, in case `randomize-inter-endpoint-pause` is set, each random pause is
taken into account when determining the timeout for establishing the connections
of a given run (see the [Run Success](#run-success) section below).

WebSocket connections are established with a given timeout for the handshake
initialization (`ws-connection-timeout-ms` in milliseconds).

PCP Association requests are sent with a given TTL (`association-ttl-s` in
seconds). Note that such TTL is only meant for the processing of the request
message; instead, the timeout for the entire association process can be
specified by `association-timeout-s` (in seconds).

WebSocket and PCP Association timings will be gathered and statistics metrics
will be provided if the `show-stats` option is flagged (defaults to `false`).

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
|  `show-stats` | bool | `false`
|  `randomize-inter-endpoint-pause` | bool | `false`
|  `inter-endpoint-pause-rng-seed` | integer | 1

Once again, ALL mandatory options must be specified in your configuration file,
in the `connection-test-parameter` object.

### Run Success

A given run is considered successful if all PCP connections are correctly
established within the time interval given by the following formula (in seconds):
```
    num-endpoints * (ws-connection-timeout-ms * 1000 + association-timeout-s) + SUM(inter-endpoint-pause)
```
In case `randomize-inter-endpoint-pause` is set, the `SUM(inter-endpoint-pause)`
term of the above formula is given by the sum of the random values that are
actually used for pausing in between connections; otherwise, if the randomize
flag is unset, such term is given by:
```
    num-endpoints * inter-endpoint-pause-ms * 1000`
```

### Result Metrics

For each run, the number of total connections and failures is reported.
More precisely, each row of the results CSV file (named
`connection_test_<date-time>.csv`) provides, in order:
 - the number of clients in each set;
 - the number of sets;
 - the number of connection failures;
 - the time to establish all the requested connections (in ms).

If `show-stats` is flagged, for each of the following timing metrics, the
mean value, the standard deviation, and the maximum value will be appended,
for a total of 16 entries for each run:
  - time to establish the TCP connection (mean value, std dev, and max value in ms);
  - time to perform the WebSocket Open Handshake (mean value, std dev, and max value in ms);
  - time to perform the PCP Association (mean value, std dev, and max value in ms);
  - duration of the PCP Session (mean value, std dev, and max value in s).

An example of output on standard out is:
```
   ~/pcp-test/build/bin ❯ ./pcp-test connection

    Connection test setup:
      4 concurrent sets (+0 per run) of 40 endpoints (+40 per run)
      2 runs, (2000 + 10 * num_endpoints) ms pause between each run
      100 ms pause between each set connection (mean value - exp. distribution)
      WebSocket connection timeout 1100 ms
      Association timeout 1 s; Association Request TTL 10 s
      keep WebSocket connections alive: yes, by pinging every 10 s

    Starting run 1: 4 concurrent sets of 40 endpoints
                    timeout for establishing all connections 1 min 28 s
                    done - closing connections and retrieving results
      [SUCCESS]  160 successful connections in 9.582 s; timing stats:
      TCP Connection: ..... mean 1.54378 ms, std dev 0.387438 ms, max 3.275 ms
      WS Open Handshake: .. mean 106.032 ms, std dev 6.52274 ms, max 142.639 ms
      PCP Association: .... mean 8.2625 ms, std dev 1.05764 ms, max 16 ms
      PCP Session: ........ mean 4.46758 s, std dev 2.59247 s, max 9.397 s (160 sessions)

    Starting run 2: 4 concurrent sets of 80 endpoints
                    timeout for establishing all connections 2 min 56 s
                    done - closing connections and retrieving results
      [SUCCESS]  320 successful connections in 18.72 s; timing stats:
      TCP Connection: ..... mean 1.56724 ms, std dev 0.379287 ms, max 3.599 ms
      WS Open Handshake: .. mean 104.493 ms, std dev 2.43615 ms, max 119.505 ms
      PCP Association: .... mean 8.10625 ms, std dev 0.633412 ms, max 15 ms
      PCP Session: ........ mean 8.69983 s, std dev 5.12135 s, max 17.89 s (320 sessions)


    Connection test: finished in 0 m 57 s

   ~/pcp-test/build/bin ❯
```

and the results file contains:
```
    40,4,0,9582,1.54378,0.387438,3.275,106.032,6.52274,142.639,8.2625,1.05764,16,4.46758,2.59247,9.397
    80,4,0,18072,1.56724,0.379287,3.599,104.493,2.43615,119.505,8.10625,0.633412,15,8.69983,5.12135,17.89
```

The related configuration of the `pcp-test.conf` file for the above test is:
```
    {
        "logfile"         : "~/logs/pcp-test.log",
        "loglevel"        : "info",
        "client-loglevel" : "trace",
        "broker-ws-uris"  : ["wss://broker.example.com:8142/pcp"],
         "connection-test-parameters" : {
            "num-runs"                       : 2,
            "inter-run-pause-ms"             : 10,
            "num-endpoints"                  : 40,
            "inter-endpoint-pause-ms"        : 1,
            "randomize-inter-endpoint-pause" : true,
            "inter-endpoint-pause-rng-seed"  : 42,
            "concurrency"                    : 4,
            "endpoints-increment"            : 40,
            "concurrency-increment"          : 0,
            "ws-connection-timeout-ms"       : 1100,
            "ws-connection-check-interval-s" : 10,
            "association-timeout-s"          : 1,
            "association-request-ttl-s"      : 10,
            "persist-connections"            : true,
            "show-stats"                     : true
        }
    }
```