step 'Add hosts entry for broker.example.com' do
  broker_ip_address = on(pcp_broker, 'facter ipaddress').stdout.chomp
  on(pcp_test, puppet("apply -e \"host { 'broker.example.com' : ip => '#{broker_ip_address}'}\""))
end
