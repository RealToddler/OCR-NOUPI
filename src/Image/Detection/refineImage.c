#include "../image.h"
#include "refineImage.h"

#include <stdio.h>

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
                if (boxes[j].height > boxes[j+1].height)
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
                if (boxes[j].width > boxes[j + 1].width)
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
                if (boxes[j].surface > boxes[j+1].surface)
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

int *compute_histogram(BoundingBox *sortedBoxes, int mod, int numBoxes) {
    int max;

    if (mod == 1)
    {
        max = sortedBoxes[numBoxes - 1].height;
    } else if (mod == 2) {
        max = sortedBoxes[numBoxes - 1].width;
    } else {
        max = sortedBoxes[numBoxes - 1].surface;
    }

    // we wont take 0 in consideration. Therefore will count
    // from 1 to max (included)

    // printf("%d\n", max);

    int *histogram = calloc(max + 1, sizeof(unsigned int));

    for (int i = 0; i < numBoxes; i++)
    {
        if (mod == 1)
        {
            if (sortedBoxes[i].height <= max)
            {
                histogram[sortedBoxes[i].height]++;
            }
        }
        else if (mod == 2)
        {
            if (sortedBoxes[i].width <= max)
            {
                histogram[sortedBoxes[i].width]++;
            }
        }
        else
        {
            if (sortedBoxes[i].surface <= max)
            {
                histogram[sortedBoxes[i].surface]++;
            }
        }
    }

    /*
    for (int i = 0; i < max + 1; i++)
    {
        printf("val = %d : %d occurences\n", i, histogram[i]);
    }
    */

    return histogram;
}

double compute_median(int *histogram)
{
    // to be made
    return 1.00;
}

double compute_average(BoundingBox *boxes, int mod, int numBoxes)
{

    double average = 0;
    for (int i = 0; i < numBoxes - 1; i++)
    {
        if (mod == 1)
        {
            average += boxes[i].height;
        }
        else if (mod == 2)
        {
            average += boxes[i].width;
        }
        else
        {
            average += boxes[i].surface;
        }
    }

    return average / numBoxes;
}