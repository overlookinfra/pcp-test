{
  :type => 'foss',
  :pre_suite => [
    'setup/common/010_InstallDependencies.rb',
    'setup/common/020_InstallPCPTest.rb',
    'setup/common/025_AddHostsEntries.rb',
    'setup/common/030_UnpackTestCertificates.rb',
    'setup/common/040_InstallPCPBroker.rb',
  ],
  :ssh => {
    :keys => ["~/.ssh/id_rsa-acceptance"],
  },
}
