#!/usr/bin/env python
# coding=utf-8
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

###
# Run me to generate the documentation!
###

# Line too long (NN/80)
# pylint: disable=C0301

"""Test tracing and isolation infrastructure.

Scripts are compartmentalized by their name:
- isolate_*.py:       Executable isolation scripts. See:
                      https://code.google.com/p/swarming/wiki/IsolateDesign
                      https://code.google.com/p/swarming/wiki/IsolateUserGuide
- isolateserver_*.py: Tools to interact with the CAD server.
- run_*.py:           Tools to run tests.
- swarm_*.py:         Swarm interaction scripts. See
                      https://code.google.com/p/swarming/wiki/SwarmingDesign
                      https://code.google.com/p/swarming/wiki/SwarmingUserGuide
- trace_*.py:         Tracing infrastructure scripts. See
                      https://code.google.com/p/swarming/wiki/TracingToolsDesign
                      https://code.google.com/p/swarming/wiki/TracingToolsUserGuide
- googletest/*.py:    Scripts specifically managing GTest executables. More
                      information about googletest can be found at
                      http://code.google.com/p/googletest/wiki/Primer

A few scripts have strict dependency rules:
- run_isolated.py and trace_inputs.py depends on no other script so they can be
  run outside the checkout.
- The pure tracing scripts (trace_*.py) do not know about isolate
  infrastructure.
- Scripts without '_test_cases' suffix do not know about GTest.
- Scripts without 'isolate_' prefix do not know about the isolation
  infrastructure.
"""

import os
import sys


def main():
  for i in sorted(os.listdir(os.path.dirname(os.path.abspath(__file__)))):
    if not i.endswith('.py') or i == 'PRESUBMIT.py':
      continue
    module = __import__(i[:-3])
    if hasattr(module, '__doc__'):
      print module.__name__
      print ''.join('  %s\n' % i for i in module.__doc__.splitlines())
  return 0


if __name__ == '__main__':
  sys.exit(main())
