#!/bin/bash
# vim: syn=sh

#@ job_name = mpi_convex_hull_%cpus%_%size%
#@ initialdir = /home/zanellas/mpi_convex_hull
#@ input = /dev/null
#@ output = run/$(job_name).out
#@ error = run/$(job_name).err
#@ class = medium
#@ job_type = parallel
#@ blocking = unlimited
#@ total_tasks = %cpus%
#@ queue

bin/mpi_convex_hull data/cloud_%size%.dat data/hull_%size%.dat data/benchmark_%cpus%_%size%.cvs
