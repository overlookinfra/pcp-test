require 'pcp_test/config_helper'

step 'Install pcp-test' do
  extend PcpTestConfigHelper

  # QENG-4205 - add CI for pcp-test
  # Install from fileserver instead
  on(pcp_test, puppet('resource package wget ensure=present'))
  on(pcp_test, 'wget http://int-resources.ops.puppetlabs.net/QA_resources/pcp_test/pcp-test-building-1.el7.x86_64.rpm')
  on(pcp_test, 'rpm -ivh pcp-test-building-1.el7.x86_64.rpm')

  on(pcp_test, 'mkdir -p /var/log/puppetlabs/pcp-test')
  on(pcp_test, 'mkdir -p /opt/puppetlabs/pcp-test/results')
end
