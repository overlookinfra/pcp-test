require 'pcp_test/config_helper.rb'
extend PcpTestConfigHelper

step 'Fetch Puppet module for pcp-broker' do
  on(pcp_broker, puppet('module install jstocks-pcp_broker'))
end

step 'Install pcp-broker' do
    apply_manifest_on(pcp_broker, <<-MANIFEST)
class { 'pcp_broker':
  run_broker   => false,
  ssl_cert     => '#{PCP_CERTS_TARGET_DIR}/broker.example.com_crt.pem',
  ssl_key      => '#{PCP_CERTS_TARGET_DIR}/broker.example.com_key.pem',
  ssl_ca_cert  => '#{PCP_CERTS_TARGET_DIR}/ca_crt.pem',
  ssl_crl_path => '#{PCP_CERTS_TARGET_DIR}/ca_crl.pem',
}
MANIFEST

  on(pcp_broker, puppet('apply -e "file { \'/var/log/puppetlabs\': ensure => directory, }"'))
end

step 'Run lein deps to download Clojure dependencies' do
  on(pcp_broker, 'cd /opt/puppet-git-repos/pcp-broker; export LEIN_ROOT=ok; lein deps')
end
