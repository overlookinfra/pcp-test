require 'net/http'
require 'openssl'
require 'socket'
require 'json'

# Some helpers for working with a pcp-broker 'lein tk' instance

def run_pcp_broker(host)
  on(host, "cd /opt/puppet-git-repos/pcp-broker; export LEIN_ROOT=ok; lein tk </dev/null >/var/log/puppetlabs/pcp-broker.log 2>&1 &")
  assert(port_open_within?(host, 8142, 60),
         "pcp-broker port 8142 not open within 1 minutes of starting the broker")
  broker_state = nil
  attempts = 0
  until broker_state == "running" or attempts == 100 do
    broker_state = get_pcp_broker_status(host)
    if broker_state != "running"
      attempts += 1
      sleep 0.5
    end
  end
  assert_equal("running", broker_state, "Shortly after startup, pcp-broker should report its state as being 'running'")
end

def kill_pcp_broker(host)
  on(host, "ps -C java -f | grep pcp-broker | sed 's/[^0-9]*//' | cut -d\\  -f1") do |result|
    pid = result.stdout.chomp
    if(pid != '')
      on(host, "kill -9 #{pid}")
    end
  end
end

def get_pcp_broker_status(host)
  uri = URI.parse("https://#{host}:8142/status/v1/services/broker-service")
  begin
    http = Net::HTTP.new(uri.host, uri.port)
    http.use_ssl = true
    http.verify_mode = OpenSSL::SSL::VERIFY_NONE
    res = http.get(uri.request_uri)
    document = JSON.load(res.body)
    return document["state"]
  rescue => e
    puts e.inspect
    return nil
  end
end
