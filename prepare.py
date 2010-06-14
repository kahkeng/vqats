#!/usr/bin/python
# Prepares a video file into a sequence of images, and writes their names out to a text file
# Kah Keng Tay

import sys, utils

if len(sys.argv) != 3:
    print "Syntax: prepare.py <video-file> <output-folder>"
    sys.exit(1)

ffmpeg = '/path/to/ffmpeg'

[video, output] = sys.argv[1:]

utils.prepare_video_file(video, output, ffmpeg)

