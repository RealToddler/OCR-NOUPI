#include "../image.h"
#include "refineImage.h"

int is_in_interval(long x, long y, long val) {
    return  val >= x && val <= y;
}

void erase(iImage *img, BoundingBox boxe) {
    for (unsigned int y = boxe.min_y; y < boxe.max_y; y++)
    {
        for (unsigned int x = boxe.min_x; x < boxe.max_x; x++)
        {
            pPixel *pixel = &img->pixels[y][x];
            pixel->r = 0;
            pixel->g = 0;
            pixel->b = 0;
        }
    }
}

BoundingBox *sort(BoundingBox *boxes, int mod, int numBoxes)
{
    BoundingBox *res = boxes;
    BoundingBox swap;

    if (mod == 1) // height
    {
        for (int i = 0; i < numBoxes; i++)
        {
            for (int j = 0; j < numBoxes - i - 1; j++)
            {
                if (boxes[j].max_y - boxes[j].min_y > boxes[j+1].max_y - boxes[j+1].min_y)
                {
                    swap = boxes[j];
                    boxes[j] = boxes[j+1];
                    boxes[j + 1] = swap;
                }
            }
        }
    } 

    if (mod == 2) // width
    {
        for (int i = 0; i < numBoxes; i++)
        {
            for (int j = 0; j < numBoxes - i - 1; j++)
            {
                if (boxes[j].max_x - boxes[j].min_x > boxes[j + 1].max_x - boxes[j + 1].min_x)
                {
                    swap = boxes[j];
                    boxes[j] = boxes[j + 1];
                    boxes[j + 1] = swap;
                }
            }
        }
    }

    if (mod == 3) // surface
    {
        for (int i = 0; i < numBoxes; i++)
        {
            for (int j = 0; j < numBoxes - i - 1; j++)
            {
                int width1 = boxes[j].max_x - boxes[j].min_x;
                int height1 = boxes[j].max_y - boxes[j].min_y;
                long surface1 = width1 * height1;

                int width2 = boxes[j+1].max_x - boxes[j+1].min_x;
                int height2 = boxes[j+1].max_y - boxes[j+1].min_y;
                long surface2 = width2 * height2;
                if (surface1 > surface2)
                {
                    swap = boxes[j];
                    boxes[j] = boxes[j + 1];
                    boxes[j + 1] = swap;
                }
            }
        }
    }

    return res;
}