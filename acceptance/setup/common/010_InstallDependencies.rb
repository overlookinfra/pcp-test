step 'Install Puppet to assist with setup' do
  hosts.each do |host|
    install_puppetlabs_dev_repo(host, 'puppet-agent', ENV['PUPPET_AGENT_SHA'], 'repo-config')
  end
  install_puppet_agent_on(hosts)
end

step 'Install java on pcp-broker' do
  on(pcp_broker, puppet('resource package java-1.8.0-openjdk-devel ensure=present'))
end
