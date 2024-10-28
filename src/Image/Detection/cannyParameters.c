#include "cannyParameters.h"
#include "../image.h"
#include "boxes.h"
#include "refineImage.h"

void find_letters_in_word(BoundingBox *boxes, int num_boxes, iImage *img) {
    Color red = {255, 0, 0};
    for (int i = 0; i < num_boxes; i++) {
        draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x,
                           boxes[i].max_y, red);
        }
}

void find_words_in_words_lists(BoundingBox *boxes, int num_boxes, iImage  *img) {
    // merge_bounding_boxes(boxes, &num_boxes, 10, 0);

    Color red = {255, 0, 0};

    for (int i = 0; i < num_boxes; i++) {
        if (boxes[i].height < 5 || boxes[i].width < 5) {
            erase(img, boxes[i]);
        } else if (boxes[i].height > boxes[i].width) {
            // erase(img, boxes[i]);
        } else {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x,
                           boxes[i].max_y, red);
        }
    }
}