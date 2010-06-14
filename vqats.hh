/*
 * Video Quality Assessment Tool using SSIM (VQATS).
 * Written by Kah Keng Tay, kahkeng AT gmail DOT com, 2008.
 *
 * This tool computes VSSIM*, a similarity score between two videos, when given
 * their image sequences as input. When comparing any two frames, we average the
 * RGB SSIM values and use that as the overall video SSIM index for the frames.
 *
 * Features:
 * - accomodates frame skips and stalls in input videos.
 * - performs such alignment automatically and quickly.
 * - minimize computation to keep things fast.
 *
 * Credit goes to:
 * - Zhou Wang for the SSIM metric. http://www.ece.uwaterloo.ca/~z70wang/research/ssim/
 * - Rabah Mehdi for original SSIM OpenCV code. http://mehdi.rabah.free.fr/
 * - John-Mark Gurney for fib package. http://resnet.uoregon.edu/~gurney_j/jmpc/fib.html
 */

#include <utility>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

// default settings for SSIM computation
#define C1  6.5025
#define C2 58.5225
#define W_Y  0.8
#define W_Cr 0.1
#define W_Cb 0.1

#define INF 1e9

#ifndef CACHE_SIZE
    #define CACHE_SIZE 20 // number of frames in cache for each video
#endif

#define INSERTED_FRAME 0.0 // score for an inserted frame, with 1.0 being best possible score
#define DELETED_FRAME 0.0 // score for an inserted frame, with 1.0 being best possible score

typedef double score_t;
typedef uint16_t video_t;
typedef uint16_t frame_t;

struct FrameData
{
    FrameData();
    ~FrameData();
    bool load(); // load the image, returns true if image is now loaded
    bool unload(); // unload the image, returns true if image got unloaded

    bool _loaded; // whether this image is loaded yet
    frame_t _index; // the frame index number
    std::string _path; // the path to the image
    IplImage *_image; // image object
#ifndef SAMPLING_SIZE
    IplImage *_image_sq, *_mu, *_mu_sq, *_sigma_sq; // other preprocessed computations
#endif
    CvSize _size;
    int _nChannels, _depth;
};

struct VideoData
{
    typedef std::vector<FrameData> FrameList;
    typedef std::list<frame_t> CacheList;
    FrameList _frames; // sequence of video frames
    CacheList _cache;
};

class VQATS;

score_t compute_video_score(VQATS& v, const video_t& video1, const video_t& video2); // returns the similarity score between two videos

class VQATS
{
    friend score_t compute_video_score(VQATS& v, const video_t& video1, const video_t& video2);
public:
    typedef std::map<video_t, VideoData> VideoMap;

    VQATS();
    ~VQATS();

    video_t load_video(std::string filename); // takes as input an ascii file that has paths to images of the sequence on separate lines

private:
    score_t compute_frame_score(const video_t& video1, const video_t& video2, const frame_t& index1, const frame_t& index2); // returns the similarity score between two frames
    bool load_video_frame(const video_t& video_index, const frame_t& frame_index); // loads the video frame into cache

    VideoMap _video_map;
    video_t _num_videos;    
};

