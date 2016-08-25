require 'pcp_test/config_helper.rb'
require 'pcp_broker/broker_helper.rb'

test_name 'Ensure that the trivial test mode works' do
  extend PcpTestConfigHelper

  teardown do
    kill_pcp_broker(pcp_broker)
  end

  step 'Create a default config file' do
    create_remote_file(pcp_test, 'pcp-test.conf', default_config_json().to_s)
  end
 
  step '(Re)start the broker' do
    kill_pcp_broker(pcp_broker)
    run_pcp_broker(pcp_broker)
  end
 
  step 'Run the trivial test' do
    on(pcp_test, "/opt/puppetlabs/pcp-test/bin/pcp-test trivial --config-file pcp-test.conf --certificates-dir #{PCP_CERTS_TARGET_DIR}",
       :accept_all_exit_codes => true) do |cmd_result|
      assert_equal(0, cmd_result.exit_code, 'Exit code from pcp-test trivial should be 0')
    end
  end
end
