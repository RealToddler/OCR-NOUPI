#include "cannyParameters.h"
#include "../image.h"
#include "boxes.h"
#include "refineImage.h"



int is_from_grid(bBoundingBox *boxes, int numBoxes, bBoundingBox box, int mod)
{
    
    if(mod == 1) //width
    {
        sort(boxes, 1, numBoxes);
        for(int i = 0; i < numBoxes; i++)
        {
            if(boxes[i].height<10 || boxes[i].width<5)
            continue;
            if(
                boxes[i].height > (box.height - (box.height / 4)) &&
                boxes[i].height < (box.height + (box.height / 4)) &&
                boxes[i].min_y < (box.min_y + (box.height / 3))&&
                boxes[i].min_y > (box.min_y - (box.height / 3))&&

                ((boxes[i].min_x != box.min_x) ||
                (boxes[i].min_y != box.min_y)) &&
                (boxes[i].min_x < ((2 * (box.width))+box.max_x) || 
                boxes[i].max_x > (box.min_x-(2 * (box.width)))))
                return 1;
        }
    }
    else if(mod == 2)//height
    {
        
        sort(boxes, 2, numBoxes);
        for(int i = 0; i < numBoxes; i++)
        {
            if(boxes[i].height<10 || boxes[i].width<5)
            continue;
            if(boxes[i].width > (box.width - (box.width / 2)) &&
               boxes[i].width < (box.width + (box.width / 3)) &&
               boxes[i].min_x > (box.min_x - (box.width / 2)) &&
               boxes[i].min_x < (box.min_x + (box.width / 3)) &&
               

               ((boxes[i].min_x != box.min_x) ||
                (boxes[i].min_y != box.min_y)) &&
                (boxes[i].max_y > (box.min_y-( 2* (box.height))) || 
                boxes[i].min_y < (box.max_y+( 2 * (box.height)))))
                return 1;
            
        }
    }
    return 0;
}







void find_grid(bBoundingBox *boxes, int num_boxes, iImage *img) {
    double wavrage = compute_median2(boxes, 1, num_boxes);
    double wavrage2 = compute_median2(boxes, 2, num_boxes);
    double savrage = compute_median2(boxes, 3, num_boxes);
    bBoundingBox *boxes_inter = sort(boxes, 1, num_boxes);

    int x_min = img->width;
    int x_max = 0;
    int y_min = img->height;
    int y_max = 0;

    for (int i = 0; i < num_boxes - 1; i++) {
        if(boxes_inter[i].height<10 || boxes_inter[i].width<5)
            continue;
        if (boxes_inter[i].surface > (savrage - (savrage / 3)) &&
            boxes_inter[i].surface < (savrage + (savrage / 3)) &&
            boxes_inter[i].height > (wavrage - (wavrage / 2)) &&
            boxes_inter[i].height < (wavrage + (wavrage / 2)) &&
            boxes_inter[i].width < (2 * (wavrage)) &&
            boxes_inter[i].width < (wavrage2 + (wavrage2 / 2)) &&
            (is_from_grid(boxes_inter, num_boxes, boxes_inter[i], 1) &&
            is_from_grid(boxes_inter, num_boxes, boxes_inter[i], 2))){
            if (boxes_inter[i].max_x > x_max)
                x_max = boxes_inter[i].max_x;
            if (boxes_inter[i].max_y > y_max)
                y_max = boxes_inter[i].max_y;
            if (boxes_inter[i].min_x < x_min)
                x_min = boxes_inter[i].min_x;
            if (boxes_inter[i].min_y < y_min)
                y_min = boxes_inter[i].min_y;
        }
        
    }
    cColor red = {255, 0, 0};

    // int h = (y_max - (y_min));
    // int w = (x_max - (x_min));
    // int surface = w * h;
    // bBoundingBox res = {x_min, x_max, y_min, y_max, h, w, surface};

    draw_rectangle(img, x_min, y_min, x_max, y_max, red);
}

void find_word_lists(bBoundingBox *boxes, int num_boxes, iImage *img) {
    double wavrage = compute_median2(boxes, 1, num_boxes);
    double wavrage2 = compute_median2(boxes, 2, num_boxes);
    bBoundingBox *boxes_inter = sort(boxes, 1, num_boxes);

    int x_min = img->width;
    int x_max = 0;
    int y_min = img->height;
    int y_max = 0;

    for (int i = 0; i < num_boxes - 1; i++) {
        if (boxes_inter[i].height > (wavrage - (wavrage / 2)) &&
            boxes_inter[i].height < (wavrage + (wavrage / 2)) &&
            boxes_inter[i].width < (2 * (wavrage)) &&
            boxes_inter[i].width < (wavrage2 + (wavrage2 / 2))) {
            if (boxes_inter[i].max_x > x_max)
                x_max = boxes_inter[i].max_x;
            if (boxes_inter[i].max_y > y_max)
                y_max = boxes_inter[i].max_y;
            if (boxes_inter[i].min_x < x_min)
                x_min = boxes_inter[i].min_x;
            if (boxes_inter[i].min_y < y_min)
                y_min = boxes_inter[i].min_y;
        }
        
    }

    int h = (y_max - (y_min));
    int w = (x_max - (x_min));
    int surface = w * h;
    bBoundingBox array = {x_min, x_max, y_min, y_max, h, w, surface};
    bBoundingBox *res = malloc(num_boxes * sizeof(bBoundingBox));
    int index = 0;

    for (int i = 0; i < num_boxes - 1; i++) {
        if ((array.max_x < boxes[i].min_x || boxes[i].max_x < array.min_x ||
             array.max_y < boxes[i].min_y || boxes[i].max_y < array.min_y) &&
            boxes[i].height < boxes[i].width && boxes[i].surface < 30000) {
            res[index] = boxes[i];
            index++;
        }
    }
    res = realloc(res, index * sizeof(bBoundingBox));
    bBoxSize res2 = {res, index};

    cColor green = {0, 255, 0};

    for (int i = 0; i < index; i++) {
        draw_rectangle(img, res2.boxes[i].min_x, res2.boxes[i].min_y, res2.boxes[i].max_x,
                       res2.boxes[i].max_y, green);
    }
}

void find_letters_in_word(bBoundingBox *boxes, int num_boxes, iImage *img) {
    double avg_surface = compute_average(boxes, 3, num_boxes);
    merge_bounding_boxes(boxes, &num_boxes, -5, 0);
    cColor red = {255, 0, 0};
    for (int i = 0; i < num_boxes; i++) {
        if (boxes[i].surface < avg_surface &&
            boxes[i].height < compute_average(boxes, 1, num_boxes)) {
            erase(img, boxes[i]);
        } else {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x,
                           boxes[i].max_y, red);
        }
    }
}

void find_words_in_words_lists(bBoundingBox *boxes, int num_boxes,
                               iImage *img) {
    merge_bounding_boxes(boxes, &num_boxes, 20, 0);

    cColor red = {255, 0, 0};

    for (int i = 0; i < num_boxes; i++) {
        if (boxes[i].height < 5 || boxes[i].width < 5) {
            erase(img, boxes[i]);
        } else if (boxes[i].height <
                   compute_average(boxes, 1, num_boxes) * 0.2) {
            erase(img, boxes[i]);
        } else {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x,
                           boxes[i].max_y, red);
        }
    }
}