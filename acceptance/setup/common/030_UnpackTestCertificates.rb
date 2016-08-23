require 'pcp_test/config_helper.rb'
extend PcpTestConfigHelper

PCP_CERTS_ARCHIVE_NAME='pcp-certificates.tar.gz'
PCP_CERTS_DIRECT_URL="https://github.com/puppetlabs/pcp-test/raw/master/test-resources/#{PCP_CERTS_ARCHIVE_NAME}"

step 'Fetch standard test certificates' do
  on(hosts, puppet('resource package wget ensure=present'))
  on(hosts, "wget #{PCP_CERTS_DIRECT_URL}")
end

step "Unpack certificates to #{PCP_CERTS_TARGET_DIR}" do
  apply_manifest_on(hosts, <<-MANIFEST)
file{'#{PCP_CERTS_TARGET_DIR}':
  ensure => 'directory',
}
MANIFEST
  on(hosts, "tar xvf #{PCP_CERTS_ARCHIVE_NAME}")
  on(hosts, "mv pcp-certificates/* #{PCP_CERTS_TARGET_DIR}/ && rm -rf pcp-certificates")
end
