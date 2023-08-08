#ifndef CLIPLINE2D_H
#define CLIPLINE2D_H

#include <QPointF>

#define X_LESS 0x08
#define X_MORE 0x04
#define X_OUT  0x0C
#define Y_LESS 0x02
#define Y_MORE 0x01
#define Y_OUT  0x03

#define FULL_OUTSIDE 0xFFFFC000
#define PART_OUTSIDE 0xFFFF8000
#define ERR1_OUTSIDE 0xFFFE8000
#define ERR2_OUTSIDE 0xFFFD8000


int  clipLine2D(QPointF &clip_min,QPointF &clip_size,QPointF &l1,QPointF &l2,QPointF *cline1,QPointF *cline2);

#endif // CLIPLINE2D_H
