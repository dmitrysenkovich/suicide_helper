# Suicide Helper

## Description
Little useless terrible kernel module I wrote for educational purposes and for fun only. I had no idea what module to write so this suicide helper was written. The only thing the module can do - it kills in a very harmful way any process trying to open device file /dev/suicide.

## If you still want to try this shit:
1. Make module and test (simple Makefile provided).
2. Run run_test.py to see it's working and even doesn't crush your machine when many tasks would like to kill themselves. Although I encourage you to try this in vm only.
