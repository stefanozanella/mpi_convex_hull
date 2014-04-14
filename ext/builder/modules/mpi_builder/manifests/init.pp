class mpi_builder {
  $build_tools = [
    'build-essential',
    'cmake',
    'clang',
    'libopenmpi1.6',
    'libopenmpi1.6-dev',
    'openmpi1.6-bin',
    'unzip',
  ]

  package { $build_tools:
    ensure => installed,
  }

  exec { 'apt-get update':
    command => '/usr/bin/apt-get update',
  }

  file { '/home/vagrant/.bashrc':
    ensure => present,
    owner  => vagrant,
    group  => vagrant,
    mode   => 0644,
    source => 'puppet:///modules/mpi_builder/dot_bashrc',
  }

  Package {
    require => Exec['apt-get update'],
  }
}
