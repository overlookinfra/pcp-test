require 'pcp_test/config_helper'

step 'Install pcp-test' do
  extend PcpTestConfigHelper

  install_puppetlabs_dev_repo(pcp_test, 'pcp-test', ENV['SUITE_COMMIT'], 'repo-config')
  install_package(pcp_test, 'pcp-test')
  
  on(pcp_test, 'mkdir -p /var/log/puppetlabs/pcp-test')
  on(pcp_test, 'mkdir -p /opt/puppetlabs/pcp-test/results')
end
