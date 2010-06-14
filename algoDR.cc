/*
 * Video Quality Assessment Tool using SSIM (VQATS).
 * Written by Kah Keng Tay, kahkeng AT gmail DOT com, 2008.
 *
 * Edit distance DP algorithm with recovery.
 */ 

#include <vector>
#include "vqats.hh"
#include "dtable.hh"

struct CellData
{
    score_t _score; // score of this cell
    score_t _sum; // cumulative score
    uint16_t _length; // path length
    char _path; // which way to reverse: 0 = diagonal, 1 = decrease i1, 2 = decrease i2
};

score_t compute_video_score(VQATS& v, const video_t& video1, const video_t& video2)
{
    uint16_t n1 = v._video_map[video1]._frames.size();
    uint16_t n2 = v._video_map[video2]._frames.size();    
    CellData** cells = new_table<CellData>(n1 + 1, n2 + 1);
    // initial values
    cells[0][0]._score = 0.0;
    cells[0][0]._sum = 0.0;
    cells[0][0]._length = 0;
    cells[0][0]._path = 0;
    for (uint16_t i1 = 1; i1 <= n1; i1++)
    {
        cells[i1][0]._score = DELETED_FRAME;
        cells[i1][0]._sum = cells[i1 - 1][0]._sum + 1.0 - cells[i1][0]._score;
        cells[i1][0]._length = i1;
        cells[i1][0]._path = 1;
    }
    for (uint16_t i2 = 1; i2 <= n2; i2++)
    {
        cells[0][i2]._score = INSERTED_FRAME;
        cells[0][i2]._sum = cells[0][i2 - 1]._sum + 1.0 - cells[0][i2]._score;
        cells[0][i2]._length = i2;
        cells[0][i2]._path = 2;
    }
    // do dynamic programming method for computing minimum average frame score
    for (uint16_t i1 = 1; i1 <= n1; i1++)
    {
        for (uint16_t i2 = 1; i2 <= n2; i2++)
        {
            score_t best_sum = INF;
            uint16_t best_length = 0;
            char best_path = 0;
            score_t best_score = 0;
            score_t new_sum = 0;
            score_t new_score = v.compute_frame_score(video1, video2, i1 - 1, i2 - 1);
            new_sum = cells[i1 - 1][i2 - 1]._sum + 1.0 - new_score;
            if (new_sum < best_sum)
            {
                best_sum = new_sum;
                best_length = cells[i1 - 1][i2 - 1]._length + 1;
                best_path = 0;
                best_score = new_score;
            }
            new_sum = cells[i1 - 1][i2]._sum + 1.0 - DELETED_FRAME;
            if (new_sum < best_sum)
            {
                best_sum = new_sum;
                best_length = cells[i1 - 1][i2]._length + 1;
                best_path = 1;
                best_score = DELETED_FRAME;
            }
            new_sum = cells[i1][i2 - 1]._sum + 1.0 - INSERTED_FRAME;
            if (new_sum < best_sum)
            {
                best_sum = new_sum;
                best_length = cells[i1][i2 - 1]._length + 1;
                best_path = 2;
                best_score = INSERTED_FRAME;
            }
            cells[i1][i2]._score = best_score;
            cells[i1][i2]._sum = best_sum;
            cells[i1][i2]._length = best_length;
            cells[i1][i2]._path = best_path;
        }
    }
    score_t sum = cells[n1][n2]._sum;
    uint16_t length = cells[n1][n2]._length;
    
#ifdef DEBUG
    for (uint16_t i1 = 0; i1 <= n1; i1++)
    {
        for (uint16_t i2 = 0; i2 <= n2; i2++)
            printf("%3.2f ", cells[i1][i2]._sum);
        printf("\n");
    }
    printf("Sum = %f\n",  sum);
    printf("Length = %d\n", length);
#endif

    // recover path
    std::vector<score_t> path_score;
    std::vector<char> path_action;
    uint16_t i1 = n1; 
    uint16_t i2 = n2;
    while (i1 > 0 || i2 > 0)
    {
        path_action.push_back(cells[i1][i2]._path);
        path_score.push_back(cells[i1][i2]._score);
        switch (cells[i1][i2]._path)
        {
            case 0: i1--; i2--; break;
            case 1: i1--; break;
            case 2: i2--; break;
        }
    }
    for (int i = path_action.size() - 1; i >= 0; i--)
    {
        const char* action = path_action[i] == 0 ? "MATCH   " : path_action[i] == 1 ? "DELETED " : "INSERTED";
        printf("Action = %s Score = %f\n", action, path_score[i]);
    }
            
    // clean up
    delete_table(cells, n1 + 1, n2 + 1);

    return 1.0 - sum / length;
}


