require 'json'

module PcpTestConfigHelper
  PCP_CERTS_TARGET_DIR='/opt/puppetlabs/pcp-certs'

  def default_config_hash
    pcp_test_config_defaults = Hash.new
    pcp_test_config_defaults['logfile']                    = '/var/log/puppetlabs/pcp-test/pcp-test.log'
    pcp_test_config_defaults['results-dir']                = '/var/log/puppetlabs/pcp-test/logs'
    pcp_test_config_defaults['loglevel']                   = 'trace'
    pcp_test_config_defaults['broker-ws-uris']             = ['wss://broker.example.com:8142/pcp/']
    pcp_test_config_defaults['certificates-dir']           = PCP_CERTS_TARGET_DIR
    pcp_test_config_defaults['connection-test-parameters'] = {
      'num-runs'                  => 1,
      'inter-run-pause-ms'        => 1000,
      'concurrency'               => 2,
      'num-endpoints'             => 10,
      'inter-endpoint-pause-ms'   => 150,
      'endpoints-increment'       => 0,
      'concurrency-increment'     => 0,
      'ws-connection-timeout-ms'  => 2000,
      'association-request-ttl-s' => 5,
      'association-timeout-s'     => 5
    }
    return pcp_test_config_defaults
  end

  def default_config_json
    default_config_hash.to_json
  end
  module_function :default_config_hash, :default_config_json
end
