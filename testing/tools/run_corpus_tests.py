#!/usr/bin/env python
# Copyright 2015 The PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import cStringIO
import functools
import multiprocessing
import optparse
import os
import re
import shutil
import subprocess
import sys

import common
import pngdiffer
import suppressor

class KeyboardInterruptError(Exception): pass

# Nomenclature:
#   x_root - "x"
#   x_filename - "x.ext"
#   x_path - "path/to/a/b/c/x.ext"
#   c_dir - "path/to/a/b/c"

def test_one_file(input_filename, source_dir, working_dir,
                  pdfium_test_path, image_differ, drmem_wrapper):
  input_path = os.path.join(source_dir, input_filename)
  pdf_path = os.path.join(working_dir, input_filename)
  # Remove any existing generated images from previous runs.
  actual_images = image_differ.GetActualFiles(
      input_filename, source_dir, working_dir)
  for image in actual_images:
    if os.path.exists(image):
      os.remove(image)

  shutil.copyfile(input_path, pdf_path)
  sys.stdout.flush()
  # add Dr. Memory wrapper if exist
  # remove .pdf suffix
  cmd_to_run = common.DrMemoryWrapper(drmem_wrapper,
                                      os.path.splitext(input_filename)[0])
  cmd_to_run.extend([pdfium_test_path, '--send-events', '--png', pdf_path])
  error = common.RunCommand(cmd_to_run)
  if error:
    print "FAILURE: " + input_filename + "; " + str(error)
    return False
  return not image_differ.HasDifferences(input_filename, source_dir, working_dir)


def test_one_file_parallel(working_dir, pdfium_test_path, image_differ,
                           test_case):
  """Wrapper function to call test_one_file() and redirect output to stdout."""
  try:
    input_filename, source_dir = test_case
    result = test_one_file(input_filename, source_dir, working_dir,
                           pdfium_test_path, image_differ, "");
    return (result, input_filename, source_dir)
  except KeyboardInterrupt:
    raise KeyboardInterruptError()


def handle_result(test_suppressor, input_filename, input_path, result,
                  surprises, failures):
  if test_suppressor.IsResultSuppressed(input_filename):
    if result:
      surprises.append(input_path)
  else:
    if not result:
      failures.append(input_path)


def main():
  parser = optparse.OptionParser()
  parser.add_option('--build-dir', default=os.path.join('out', 'Debug'),
                    help='relative path from the base source directory')
  parser.add_option('-j', default=multiprocessing.cpu_count(),
                    dest='num_workers', type='int',
                    help='run NUM_WORKERS jobs in parallel')
  parser.add_option('--wrapper', default='', dest="wrapper",
                    help='Dr. Memory wrapper for running test under Dr. Memory')
  options, args = parser.parse_args()
  finder = common.DirectoryFinder(options.build_dir)
  pdfium_test_path = finder.ExecutablePath('pdfium_test')
  if not os.path.exists(pdfium_test_path):
    print "FAILURE: Can't find test executable '%s'" % pdfium_test_path
    print "Use --build-dir to specify its location."
    return 1
  working_dir = finder.WorkingDir(os.path.join('testing', 'corpus'))
  if not os.path.exists(working_dir):
    os.makedirs(working_dir)

  feature_string = subprocess.check_output([pdfium_test_path, '--show-config'])
  test_suppressor = suppressor.Suppressor(finder, feature_string)
  image_differ = pngdiffer.PNGDiffer(finder)

  # test files are under .../pdfium/testing/corpus.
  failures = []
  surprises = []
  walk_from_dir = finder.TestingDir('corpus');
  input_file_re = re.compile('^[a-zA-Z0-9_.]+[.]pdf$')
  test_cases = []

  if len(args):
    for file_name in args:
      input_path = os.path.join(walk_from_dir, file_name)
      if not os.path.isfile(input_path):
        print "Can't find test file '%s'" % file_name
        return 1

      test_cases.append((os.path.basename(input_path),
                         os.path.dirname(input_path)))
  else:
    for source_dir, _, filename_list in os.walk(walk_from_dir):
      for input_filename in filename_list:
        if input_file_re.match(input_filename):
          input_path = os.path.join(source_dir, input_filename)
          if not test_suppressor.IsExecutionSuppressed(input_path):
            if os.path.isfile(input_path):
              test_cases.append((input_filename, source_dir))

  if options.num_workers > 1 and len(test_cases) > 1:
    try:
      pool = multiprocessing.Pool(options.num_workers)
      worker_func = functools.partial(test_one_file_parallel, working_dir,
                                      pdfium_test_path, image_differ)
      worker_results = pool.imap(worker_func, test_cases)
      for worker_result in worker_results:
        result, input_filename, source_dir = worker_result
        input_path = os.path.join(source_dir, input_filename)
        handle_result(test_suppressor, input_filename, input_path, result,
                      surprises, failures)
      pool.close()
    except KeyboardInterrupt:
      pool.terminate()
    finally:
      pool.join()
  else:
    for test_case in test_cases:
      input_filename, source_dir = test_case
      result = test_one_file(input_filename, source_dir, working_dir,
                             pdfium_test_path, image_differ,
                             options.wrapper)
      handle_result(test_suppressor, input_filename, input_path, result,
                    surprises, failures)

  if surprises:
    surprises.sort()
    print '\n\nUnexpected Successes:'
    for surprise in surprises:
      print surprise;

  if failures:
    failures.sort()
    print '\n\nSummary of Failures:'
    for failure in failures:
      print failure
    return 1

  return 0


if __name__ == '__main__':
  sys.exit(main())
