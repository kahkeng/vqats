/*
 * Video Quality Assessment Tool using SSIM (VQATS).
 * Written by Kah Keng Tay, kahkeng AT gmail DOT com, 2008.
 *
 * See header file for complete credits.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include "vqats.hh"

#ifdef SAMPLING_SIZE
#include <stdlib.h>
#include "cvmat.hh" // for modified CvScalar operators
#endif

// Macros from http://en.wikipedia.org/wiki/C_preprocessor to prevent side effects
#define min(a,b) \
       ({ typeof (a) _a = (a); \
           typeof (b) _b = (b); \
         _a < _b ? _a : _b; })
#define max(a,b) \
       ({ typeof (a) _a = (a); \
           typeof (b) _b = (b); \
         _a > _b ? _a : _b; })

FrameData::FrameData()
    : _loaded(false), _index(0), _image(NULL)
{
#ifndef SAMPLING_SIZE
    _image_sq = _mu = _mu_sq = _sigma_sq = NULL;
#endif
}

FrameData::~FrameData()
{
    if (unload())
    {
#ifdef DEBUG
        std::cout << "Deleting frame " << _path << std::endl;
#endif
    }
}

bool
FrameData::load()
{
    if (!_loaded)
    {
#ifdef DEBUG
        std::cout << "Loading frame " << _path << std::endl;
#endif

        _loaded = true;
        _image = cvLoadImage(_path.c_str());
        if (_image == NULL)
            return false;

        // precompute values that deal with only a single image
        // convert the image to YCrCb color space, keeping the same depth as before (most likely IPL_DEPTH_8U)
        _nChannels = _image->nChannels;
        _size = cvSize(_image->width, _image->height);
        IplImage* temp = cvCreateImage(_size, _image->depth, _nChannels);
        cvCvtColor(_image, temp, CV_BGR2YCrCb);
        cvReleaseImage(&_image);

        // now convert it to IPL_DEPTH_32F to overcome the 0..255 range.
        _depth = IPL_DEPTH_32F;
        _image = cvCreateImage(_size, _depth, _nChannels);
        cvConvert(temp, _image);
        cvReleaseImage(&temp);

#ifndef SAMPLING_SIZE
        _image_sq = cvCreateImage(_size, _depth, _nChannels);
        cvPow(_image, _image_sq, 2);

        _mu = cvCreateImage(_size, _depth, _nChannels);
        _mu_sq = cvCreateImage(_size, _depth, _nChannels);
        _sigma_sq = cvCreateImage(_size, _depth, _nChannels);

        cvSmooth(_image, _mu, CV_GAUSSIAN, 11, 11, 1.5);
        cvPow(_mu, _mu_sq, 2);
        cvSmooth(_image_sq, _sigma_sq, CV_GAUSSIAN, 11, 11, 1.5);
        cvAddWeighted(_sigma_sq, 1, _mu_sq, -1, 0, _sigma_sq);
#endif
    }
    return true;
}

bool
FrameData::unload()
{
    if (_loaded)
    {
#ifdef DEBUG
        std::cout << "Unloading frame " << _path << std::endl;
#endif

        _loaded = false;
        if (_image != NULL) cvReleaseImage(&_image);
#ifndef SAMPLING_SIZE
        if (_image_sq != NULL) cvReleaseImage(&_image_sq);
        if (_mu != NULL) cvReleaseImage(&_mu);
        if (_mu_sq != NULL) cvReleaseImage(&_mu_sq);
        if (_sigma_sq != NULL) cvReleaseImage(&_sigma_sq);
#endif
        return true;
    }
    return false;
}

VQATS::VQATS()
    : _num_videos(0)
{
}

VQATS::~VQATS()
{
}

video_t
VQATS::load_video(std::string filename)
{
    frame_t index = 0;
    // read our ascii file and save image paths
    std::fstream fs(filename.c_str(), std::fstream::in);
    if (fs.fail())
    {
        std::cout << "Unable to load video file " << filename << std::endl;
        return 0;
    }
    video_t id = ++_num_videos;
    while (!fs.eof())
    {
        std::string s;
        fs >> s;      
        if (s.length() > 0)
        {
            FrameData frame_data;
            frame_data._path = s;
            frame_data._index = index++;
            _video_map[id]._frames.push_back(frame_data);
#ifdef DEBUG
            std::cout << "Initialized frame " << s << " for sequence " << filename << std::endl;
#endif
        }
    }
    fs.close();
    return id;
}

bool
VQATS::load_video_frame(const video_t& video_index, const frame_t& frame_index)
{
    VideoData& video = _video_map[video_index];
    // we have a LRU cache replacement policy
    // check if this frame is already in the cache
    bool found = false;
    VideoData::CacheList::iterator slot = video._cache.begin();
    // if (video_index != 1) for (int i = 0; i < (int)video._cache.size() - 1; i++) slot++; // do MRU for videos other than first one
    for (VideoData::CacheList::iterator it = video._cache.begin(); it != video._cache.end(); ++it)
    {
        if (*it == frame_index)
        {
            slot = it;
            found = true;
            break;
        }
    }
    if (video._cache.size() >= CACHE_SIZE || found)
    {
        if (!found)
            video._frames[*slot].unload();
        video._cache.erase(slot);
    }
    video._cache.push_back(frame_index);
#ifdef DEBUG
    // debug output for cache list
    std::cout << "Cache List: ";
    for (VideoData::CacheList::iterator it = video._cache.begin(); it != video._cache.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
#endif
    if (!found)
        return video._frames[frame_index].load();
    else
        return true; // was already in cache
}

score_t
VQATS::compute_frame_score(const video_t& video1, const video_t& video2, const frame_t& index1, const frame_t& index2)
{
    if (!load_video_frame(video1, index1)) return 0.0;
    if (!load_video_frame(video2, index2)) return 0.0;

    FrameData& frame1 = _video_map[video1]._frames[index1];
    FrameData& frame2 = _video_map[video2]._frames[index2];
    
    // assert some properties about the frames we are comparing
    assert(frame1._size.width == frame2._size.width);
    assert(frame1._size.height == frame2._size.height);
    assert(frame1._depth == frame2._depth);
    assert(frame1._nChannels == frame2._nChannels);

#ifdef SAMPLING_SIZE

    // compute a random seed based on paths of frames. we try to make sure it is commutative.
    unsigned int seed1 = 0, seed2 = 0, m = 30011; // just some prime
    for (std::string::iterator it = frame1._path.begin(); it != frame1._path.end(); ++it)
        seed1 = seed1 * m + *it;
    for (std::string::iterator it = frame2._path.begin(); it != frame2._path.end(); ++it)
        seed2 = seed2 * m + *it;
    srand(seed1 + seed2); // initialize with this random seed

    unsigned int sx = SAMPLING_WIN_X, sy = SAMPLING_WIN_Y, // window size
                 rangex = frame1._size.width - sx + 1, rangey = frame1._size.height - sy + 1; // range of valid x and y
    CvMat *image1 = cvCreateMat(sx, sy, CV_32FC3), *image2 = cvCreateMat(sx, sy, CV_32FC3),
          *image1_sq = cvCreateMat(sx, sy, CV_32FC3), *image2_sq = cvCreateMat(sx, sy, CV_32FC3),
          *image_product = cvCreateMat(sx, sy, CV_32FC3);
    
    CvScalar index_scalar = cvScalar(0.0, 0.0, 0.0, 0.0);
    double total_weight = 0.0;
    for (unsigned int i = 0; i < SAMPLING_SIZE; i++)
    {
        unsigned int rx = rand() % rangex, ry = rand() % rangey; // random sample position
        
        image1 = cvGetSubRect(frame1._image, image1, cvRect(rx, ry, sx, sy));
        image2 = cvGetSubRect(frame2._image, image2, cvRect(rx, ry, sx, sy));
        cvPow(image1, image1_sq, 2);
        cvPow(image2, image2_sq, 2);
        CvScalar mu1 = cvAvg(image1), mu2 = cvAvg(image2), 
                 mu1_sq = mu1 * mu1, mu2_sq = mu2 * mu2,
                 sigma1_sq = cvAvg(image1_sq) - mu1_sq, sigma2_sq = cvAvg(image2_sq) - mu2_sq;
        cvMul(image1, image2, image_product, 1.0);
        CvScalar mu_product = mu1 * mu2;
        CvScalar sigma_cross = cvAvg(image_product) - mu_product;
        
        CvScalar numerator = (2.0 * mu_product + C1) * (2.0 * sigma_cross + C2);
        CvScalar denominator = (mu1_sq + mu2_sq + C1) * (sigma1_sq + sigma2_sq + C2);
        CvScalar ssim = numerator / denominator;

#ifdef SAMPLING_LUMINANCE_WEIGHTING
        double w = mu1.val[0] <= 40.1 ? 0.01 : // we want to avoid zero weights
                   mu1.val[0] >= 50 ? 1 :
                   (mu1.val[0] - 40) / 10;
        index_scalar += ssim * w;
        total_weight += w;
#else        
        index_scalar += ssim;
        total_weight += 1;
#endif
    }
    
    index_scalar /= total_weight;
    
    // clean up
    cvReleaseMat(&image1);
    cvReleaseMat(&image2);
    cvReleaseMat(&image1_sq);
    cvReleaseMat(&image2_sq);
    cvReleaseMat(&image_product);

#else

    // perform SSIM computation
    IplImage *image_product = NULL,
             *mu_product = NULL,
             *sigma_cross = NULL,
             *numerator = NULL,
             *denominator = NULL,
             *ssim_map = NULL,
             *temp1 = NULL,
             *temp2 = NULL;

    CvSize size = frame1._size;
    int depth = frame1._depth;
    int nChannels = frame1._nChannels;

    image_product = cvCreateImage(size, depth, nChannels);
    mu_product = cvCreateImage(size, depth, nChannels);
    sigma_cross = cvCreateImage(size, depth, nChannels);
    temp1 = cvCreateImage(size, depth, nChannels);
    temp2 = cvCreateImage(size, depth, nChannels);
    numerator = cvCreateImage(size, depth, nChannels);
    denominator = cvCreateImage(size, depth, nChannels);
    ssim_map = cvCreateImage(size, depth, nChannels);

    cvMul(frame1._image, frame2._image, image_product, 1);
    cvMul(frame1._mu, frame2._mu, mu_product, 2); // scale by 2 to save one computation. note: mu_product is twice its actual value.

    cvSmooth(image_product, sigma_cross, CV_GAUSSIAN, 11, 11, 1.5);
    cvAddWeighted(sigma_cross, 2, mu_product, -1, C2, temp2); // scale by 2, add C2 to save two computations. note: mu_product is twice actual value, due to above.

    cvAddS(mu_product, cvScalarAll(C1), temp1); // note: mu_product is twice actual value, due to above.
    cvMul(temp1, temp2, numerator, 1);

    cvAdd(frame1._mu_sq, frame2._mu_sq, temp1);
    cvAddS(temp1, cvScalarAll(C1), temp1);

    cvAdd(frame1._sigma_sq, frame2._sigma_sq, temp2);
    cvAddS(temp2, cvScalarAll(C2), temp2);

    cvMul(temp1, temp2, denominator, 1);

    cvDiv(numerator, denominator, ssim_map, 1);
    CvScalar index_scalar = cvAvg(ssim_map);

    // cleanup
    cvReleaseImage(&image_product);
    cvReleaseImage(&mu_product);
    cvReleaseImage(&sigma_cross);
    cvReleaseImage(&numerator);
    cvReleaseImage(&denominator);
    cvReleaseImage(&ssim_map);
    cvReleaseImage(&temp1);
    cvReleaseImage(&temp2);

#endif

#ifdef DEBUG
    std::cout << "Comparing " << frame1._path << " with " << frame2._path 
                << ": ("  << index_scalar.val[0] 
                << "," << index_scalar.val[1] 
                << "," << index_scalar.val[2] << ")" << std::endl;
#endif

    return index_scalar.val[0] * W_Y + index_scalar.val[1] * W_Cr + index_scalar.val[2] * W_Cb;
}

