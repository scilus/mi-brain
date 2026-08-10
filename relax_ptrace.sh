#!/bin/bash
sudo sysctl kernel.yama.ptrace_scope=0
cat /proc/sys/kernel/yama/ptrace_scope