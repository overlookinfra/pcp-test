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

If `show-stats` is flagged, for each of the following timing metrics, the
mean value, the standard deviation, and the maximum value will be appended:
  - time to establish the TCP connection (in ms);
  - time to perform the WebSocket Open Handshake (in ms);
  - time to perform the PCP Association (in ms);
  - duration of the PCP Session (in s).

An example of output on standard out is:
```
   ~/pcp-test/build/bin ❯ ./pcp-test connection

Connection test setup:
  2 concurrent sets (+0 per run) of 2 endpoints (+50 per run)
  2 runs, (50 + 50 * num_endpoints) ms pause between each run
  10 ms pause between each endpoint connection
  WebSocket connection timeout 500 ms
  Association timeout 1 s; Association Request TTL 1 s
  keep WebSocket connections alive: yes, by pinging every 60 s

Starting run 1: 2 concurrent sets of 2 endpoints; timeout for each set 4 s
  [SUCCESS]  4 successful connections; timing stats:
  TCP Connection: ..... mean 2.55275 ms, std dev 0.674217 ms, max 3.073 ms
  WS Open Handshake: .. mean 110.722 ms, std dev 2.43718 ms, max 114.857 ms
  PCP Association: .... mean 16.5 ms, std dev 3.5 ms, max 20 ms
  PCP Session: ........ mean 0.1265 s, std dev 0.1155 s, max 0.242 s (4 sessions)

Starting run 2: 2 concurrent sets of 52 endpoints; timeout for each set 104 s
  [SUCCESS]  104 successful connections; timing stats:
  TCP Connection: ..... mean 2.95469 ms, std dev 0.57252 ms, max 4.029 ms
  WS Open Handshake: .. mean 112.555 ms, std dev 2.86086 ms, max 119.559 ms
  PCP Association: .... mean 13.1923 ms, std dev 1.22535 ms, max 17 ms
  PCP Session: ........ mean 5.90134 s, std dev 3.46139 s, max 11.781 s (104 sessions)


Connection test: finished in 0 m 28 s

   ~/pcp-test/build/bin ❯
```

and the results file contains:
```
    2,2,02.55275,6.74217,3.073,110.722,2.43718,114.857,16.5,3.5,20,0.1265,0.1155,0.242
    52,2,02.95469,5.7252,4.029,112.555,2.86086,119.559,13.1923,1.22535,17,5.90134,3.46139,11.781
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
            "inter-run-pause-ms"             : 50,
            "num-endpoints"                  : 2,
            "inter-endpoint-pause-ms"        : 10,
            "concurrency"                    : 2,
            "endpoints-increment"            : 50,
            "concurrency-increment"          : 0,
            "ws-connection-timeout-ms"       : 500,
            "ws-connection-check-interval-s" : 30,
            "association-timeout-s"          : 1,
            "association-request-ttl-s"      : 1,
            "persist-connections"            : true,
            "show-stats"                     : true
        }
    }
```