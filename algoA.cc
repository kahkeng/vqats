/*
 * Video Quality Assessment Tool using SSIM (VQATS).
 * Written by Kah Keng Tay, kahkeng AT gmail DOT com, 2008.
 *
 * A* algorithm.
 */ 

#include "vqats.hh"
#include "dtable.hh"
#include "fib.h"

enum state_t
{
    NONE = 0,
    OPEN,
    CLOSED
};

struct QueueElement
{
    QueueElement(frame_t i1, frame_t i2): _i1(i1), _i2(i2), _estimate(0), _sum(0), _length(0) { }
    frame_t _i1, _i2;
    score_t _estimate; // estimated score
    score_t _sum; // cumulative score
    uint16_t _length; // path length
};

static inline int element_comparator(void* x, void* y)
{
    QueueElement* a = (QueueElement*)x;
    QueueElement* b = (QueueElement*)y;
    if (a->_estimate < b->_estimate) return -1;
    else if (a->_estimate > b->_estimate) return 1;
    else return 0;
}

static inline score_t element_heuristic(frame_t i1, frame_t i2, frame_t j1, frame_t j2)
{
    frame_t a1 = abs(i1 - j1);
    frame_t a2 = abs(i2 - j2);
    if (a1 > a2) return (a1 - a2) * (1.0 - DELETED_FRAME);
    else return (a2 - a1) * (1.0 - INSERTED_FRAME);
}

score_t compute_video_score(VQATS& v, const video_t& video1, const video_t& video2)
{
    frame_t n1 = v._video_map[video1]._frames.size();
    frame_t n2 = v._video_map[video2]._frames.size();    
    void*** start_he = new_table<void*>(n1 + 1, n2 + 1, NULL); // heap elements
    QueueElement*** start_qe = new_table<QueueElement*>(n1 + 1, n2 + 1, NULL); // queue elements
    state_t** start_state = new_table<state_t>(n1 + 1, n2 + 1, NONE); // node states

    // initial values
    start_qe[0][0] = new QueueElement(0, 0);
    start_state[0][0] = OPEN;
    
    // initial queues
    struct fibheap* start_heap = fh_makeheap();
    fh_setcmp(start_heap, element_comparator);
    start_he[0][0] = fh_insert(start_heap, (void*)start_qe[0][0]);

    score_t sum = 0;
    uint16_t length = 0;
    int ninserts = 0;
#ifdef FH_STATS
    printf("maxinserts = %d\n", (n1+1)*(n2+1)); fflush(stdout);
#endif

    while (true)
    {
        QueueElement *qs, *qe;
        int i1, i2;
        score_t s;

        // check start frontier
        qs = (QueueElement*)fh_extractmin(start_heap);
        assert(qs); // there is always a path
        assert(start_state[qs->_i1][qs->_i2] == OPEN); // this node should be open
#ifdef DEBUG
        printf("extract from start: (%d,%d) est=%.4f sum=%.4f len=%d\n", qs->_i1, qs->_i2, qs->_estimate, qs->_sum, qs->_length);
#endif
        start_state[qs->_i1][qs->_i2] = CLOSED;
        if (qs->_i1 == n1 && qs->_i2 == n2) // we have found an optimal path
        {
            sum = qs->_sum;
            length = qs->_length;
            break;
        }
        // expand this node outward
        for (int dir = 0; dir < 3; dir++)
        {
            switch (dir)
            {
            case 0: i1 = qs->_i1 + 1; i2 = qs->_i2 + 1; break;
            case 1: i1 = qs->_i1; i2 = qs->_i2 + 1; break; // insert frame
            case 2: i1 = qs->_i1 + 1; i2 = qs->_i2; break; // delete frame
            }
            if (i1 <= n1 && i2 <= n2 && start_state[i1][i2] != CLOSED)
            {
                switch (dir)
                {
                case 0: s = 1.0 - v.compute_frame_score(video1, video2, i1 - 1, i2 - 1); break;
                case 1: s = 1.0 - INSERTED_FRAME; break;
                case 2: s = 1.0 - DELETED_FRAME; break;
                }
                qe = new QueueElement(i1, i2);
                qe->_sum = qs->_sum + s;
                qe->_length = qs->_length + 1;
                qe->_estimate = qe->_sum + element_heuristic(i1, i2, n1, n2);
                switch (start_state[i1][i2])
                {
                case NONE:
#ifdef DEBUG
                    printf("expanding start: (%d,%d) est=%.4f sum=%.4f len=%d\n", i1, i2, qe->_estimate, qe->_sum, qe->_length);
#endif
#ifdef FH_STATS
                    ninserts = fh_ninserts(start_heap);
                    if (ninserts % 10000 == 0) { printf("%d %d %d\n", ninserts, fh_nextracts(start_heap), fh_maxn(start_heap)); fflush(stdout); }
#endif
                    assert(start_qe[i1][i2] == NULL);
                    start_he[i1][i2] = fh_insert(start_heap, (void*)qe);
                    start_qe[i1][i2] = qe;
                    start_state[i1][i2] = OPEN;
                    break;
                case OPEN:
#ifdef DEBUG
                    printf("updating start: (%d,%d) est=%.4f sum=%.4f len=%d\n", i1, i2, qe->_estimate, qe->_sum, qe->_length);
#endif
                    assert(start_qe[i1][i2] != NULL);
                    if (qe->_estimate < start_qe[i1][i2]->_estimate)
                    {
                        start_qe[i1][i2] = qe;
                        qe = (QueueElement*)fh_replacedata(start_heap, (struct fibheap_el*)start_he[i1][i2], (void*)qe);
                    }
                    delete qe;
                    break;
                default:
                    assert(false); // should not reach here
                    break;
                }
            }
        }
    }
    assert(length > 0);

#ifdef DEBUG
    printf("Sum = %f\n", sum);
    printf("Length = %d\n", length);
#endif
#ifdef FH_STATS
    printf("MaxN = %d\n", fh_maxn(start_heap));
    printf("Inserts = %d\n", fh_ninserts(start_heap));
    printf("Extracts = %d\n", fh_nextracts(start_heap));
#endif
    fflush(stdout);

    // clean up
    fh_deleteheap(start_heap);
    for (uint16_t i1 = 0; i1 <= n1; i1++)
    {
        for (uint16_t i2 = 0; i2 <= n2; i2++)
        {
            if (start_qe[i1][i2])
                delete start_qe[i1][i2];
        }
    }
    delete_table(start_he, n1 + 1, n2 + 1);
    delete_table(start_qe, n1 + 1, n2 + 1);
    delete_table(start_state, n1 + 1, n2 + 1);

    return 1.0 - sum / length;
}


