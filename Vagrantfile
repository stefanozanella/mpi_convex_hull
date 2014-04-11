# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "pulse00/ubuntu-saucy"

  config.vm.provider :virtualbox do |vb|
    vb.customize ["modifyvm", :id, "--memory", "1024"]
    vb.customize ["modifyvm", :id, "--cpus", "4"]
  end

  config.vm.provision :puppet do |puppet|
    puppet.manifests_path = "ext/builder/manifests"
    puppet.module_path = "ext/builder/modules"
    puppet.manifest_file  = "init.pp"
  end

  config.vbguest.auto_update = false
end
