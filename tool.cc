#include <stdio.h>
#include "vqats.hh"

// takes as input two text files containing paths to images in a video sequence, and prints out the VQATS similarity score.

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("Syntax: %s <video-text-file1> <video-text-file2>\n\n", argv[0]);
        return -1;
    }

    VQATS v;
    video_t v1 = v.load_video(argv[1]);
    video_t v2 = v.load_video(argv[2]);
    if (v1 == 0 || v2 == 0) return -1;
    score_t s = compute_video_score(v, v1, v2);
    printf("Score: %.4f\n", s);
    
    return 0;
}

