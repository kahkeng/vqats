#!/usr/bin/python
# Utility functions.
# Kah Keng Tay

import sys, popen2, stat, os, math

def shell_exec(s):
    r, w = popen2.popen4(s)
    output = r.readlines()
    r.close()
    w.close()
    return output

def file_exists(path):
    return os.path.exists(path) and os.path.isfile(path)

def read_file_lines(filename):
    f = open(filename, 'r')
    line = f.readline()
    while line:
        yield line.strip()
        line = f.readline()
    f.close()

def get_file_size(filename):
    try:
        return os.lstat(filename)[stat.ST_SIZE]
    except:
        return 0

def prepare_video_file(filename, outputdir="/tmp", ffmpeg="ffmpeg"):
    d = {}
    [d['video'], d['output'], d['ffmpeg']] = [filename, outputdir, ffmpeg]
    d['prefix'] = d['video']
    d['prefix'] = d['prefix'][d['prefix'].rfind('/')+1:] # get video after last slash
    d['prefix'] = d['prefix'][:d['prefix'].rindex('.')] # get video until the last dot

    shell_exec('%(ffmpeg)s -i %(video)s -f image2 %(output)s/%(prefix)s_%%d.bmp' % d)
    output = shell_exec('ls -l %(output)s/%(prefix)s_*.bmp | grep -c %(prefix)s' % d)
    d['num_frames'] = int(output[0])

    f = open('%(output)s/%(prefix)s' % d, 'w')
    for i in range(1, d['num_frames']+1):
        d['i'] = i
        f.write('%(output)s/%(prefix)s_%(i)d.bmp\n' % d)
    f.close()

    #print 'Prepared %(num_frames)d frames for video %(video)s.' % d

    # Return the list of frames
    return '%(output)s/%(prefix)s' % d

