#include "cannyParameters.h"
#include "../image.h"
#include "boxes.h"
#include "refineImage.h"

void find_grid(BoundingBox *boxes, int num_boxes, iImage *img) {
}

void find_letters_in_word(BoundingBox *boxes, int num_boxes, iImage *img) {
    double avg_surface = compute_average(boxes, 3, num_boxes);
    merge_bounding_boxes(boxes, &num_boxes, -5, 0);
    Color red = {255, 0, 0};
    for (int i = 0; i < num_boxes; i++) {
        if (boxes[i].surface < avg_surface && boxes[i].height < compute_average(boxes, 1, num_boxes))
        {
            erase(img, boxes[i]);
        } else {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x,
                           boxes[i].max_y, red);
        }
    }
}

void find_words_in_words_lists(BoundingBox *boxes, int num_boxes, iImage *img) {
    merge_bounding_boxes(boxes, &num_boxes, 20, 0);

    Color red = {255, 0, 0};

    for (int i = 0; i < num_boxes; i++) {
        if (boxes[i].height < 5 || boxes[i].width < 5) {
            erase(img, boxes[i]);
        } else if (boxes[i].height < compute_average(boxes, 1, num_boxes) * 0.2) {
            erase(img, boxes[i]);
         } else {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x,
                           boxes[i].max_y, red);
        }
    }
}