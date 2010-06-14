#!/usr/bin/python
# Given input video files, we prepare them, evaluate their quality, print the result and clean up.
# Kah Keng Tay

import sys, utils

if len(sys.argv) != 3:
    print "Syntax: eval.py <refvideo> <testvideo>"
    sys.exit(1)

d = {}
[d['refvideo'], d['testvideo']] = sys.argv[1:]
d['ffmpeg'] = '/path/to/ffmpeg'
d['vqats'] = '/path/to/vqatsAx'
d['resize'] = '160x120'
d['suffix'] = 'resized.mpg'

if len(d['resize']) > 0:
    # check if resized version of refvideo exists, if so, no need to recreate
    if not utils.file_exists('%(refvideo)s.%(suffix)s' % d):
        utils.shell_exec('%(ffmpeg)s -i %(refvideo)s -s 160x120 %(refvideo)s.%(suffix)s > /dev/null' % d)
    # check if resized version of testvideo exists, if so, delete it first before recreating
    if utils.file_exists('%(testvideo)s.%(suffix)s' % d):
        utils.shell_exec('rm -f %(testvideo)s.%(suffix)s' % d)
    # resize the testvideo again
    utils.shell_exec('%(ffmpeg)s -i %(testvideo)s -s 160x120 %(testvideo)s.%(suffix)s > /dev/null' % d)
    d['refvideo'] = '%(refvideo)s.%(suffix)s' % d
    d['testvideo'] = '%(testvideo)s.%(suffix)s' % d

# create temporary directories to prepare the videos
utils.shell_exec('mkdir -p %(refvideo)s.tmp' % d)
utils.shell_exec('mkdir -p %(testvideo)s.tmp' % d)

try:
    # prepare videos
    d['ref'] = utils.prepare_video_file(d['refvideo'], d['refvideo']+'.tmp', d['ffmpeg'])
    d['test'] = utils.prepare_video_file(d['testvideo'], d['testvideo']+'.tmp', d['ffmpeg'])

    # run evaluation
    output = utils.shell_exec('%(vqats)s %(ref)s %(test)s' % d)
    print "vqats = " + output[-1].strip().split(' ')[-1]
except:
    print "vqats = -1"
    pass

# clean up
utils.shell_exec('rm -rf %(refvideo)s.tmp' % d)
utils.shell_exec('rm -rf %(testvideo)s.tmp' % d)
if len(d['resize']) > 0:
    utils.shell_exec('rm -f %(testvideo)s' % d) # this is the resized file

