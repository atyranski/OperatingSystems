#include "configs.h"

int threads;
const char *mode;
const char *img_input;
const char *img_output; 
Image *image;

Image *getImage(const char* path){
    Image *image = calloc(1, sizeof(Image));
    FILE *file;
    size_t buffer_size = 128;
    char *line = calloc(buffer_size, sizeof(char));
    
            printf("%d\n", 1);

    if((file = fopen(path, "r") == NULL)){
        error("COULDNT_OPEN_FILE", "program occured problem with opening file");
        exit(1);
    }

            printf("%d\n", 2);

    if(!fscanf(file, "%s", *line) || strcmp(line, "P2") != 0){
        error("INVALID_FILE_FORMAT", "file from provided path is incorrect, becouse it doesn't start with 'P2' header");
        exit(1);
    }

            printf("%d\n", 3);

    fscanf(file, "%d", line);
    image->width = atoi(line);

            printf("%d\n", 4);

    fscanf(file, "%d", line);
    image->height = atoi(line);

            printf("%d\n", 5);    

    fscanf(file, "%d", line);
    image->maxColor = atoi(line);

            printf("%d\n", 6);

    image->colors = calloc(
        image->width * image->height,
        sizeof(unsigned char)
    );

            printf("%d\n", 7);

    for(int col=0; col<image->height; col++){
        for(int row=0; row<image->width; row++){
            int color;
            fscanf(file, "%d", &color);

            image->colors[col * image->width + row] = (unsigned char) color;
        }
    }
    
    return image;
}

// ---- Main program
int main(int argc, char **argv){

    // Validation of arguments
    if(argc != 5
        || atoi(argv[1]) < 0
        || (strcmp("numbers", argv[2]) != 0  
            && strcmp("blocks", argv[2]) != 0)){
        error("INCORRECT_ARGUMENTS", "[threads-amount] [mode] [image-input-path] [image-output-file]");
        return RETURN_INCORRECT_ARGUMENTS;
    }
    
    // Initialization
    threads = atoi(argv[1]);
    mode = argv[2];
    img_input = argv[3];
    img_output = argv[4];

    // Loading image
    image = getImage(img_input);

    printf("%d\n", image->height);

    free(image);

    return RETURN_SUCCESS;
}