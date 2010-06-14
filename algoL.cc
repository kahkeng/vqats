/*
 * Video Quality Assessment Tool using SSIM (VQATS).
 * Written by Kah Keng Tay, kahkeng AT gmail DOT com, 2008.
 *
 * No alignment.
 */ 

#include <vector>
#include "vqats.hh"

#define min(a,b) ({ typeof(a) _a = (a); typeof(b) _b = (b); _a < _b ? _a : _b; })
#define max(a,b) ({ typeof(a) _a = (a); typeof(b) _b = (b); _a > _b ? _a : _b; })

score_t compute_video_score(VQATS& v, const video_t& video1, const video_t& video2)
{
    uint16_t n1 = v._video_map[video1]._frames.size();
    uint16_t n2 = v._video_map[video2]._frames.size();    
    score_t sum = 0;
    for (uint16_t i = 0; i < min(n1, n2); i++)
    {
        score_t frame_score = v.compute_frame_score(video1, video2, i, i);
        printf("FrameScore: %3.2f\n", frame_score);
        sum += frame_score;
    }
    return sum / max(n1, n2);
}


