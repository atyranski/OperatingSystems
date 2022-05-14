#include "configs.h"

int thread_amount;
const char *mode;
const char *img_input;
const char *img_output; 
Image *image;
pthread_t *threads;

int getAreaStart(int k, int N, int m){
    return k * ceil(N/m);
}

int getAreaEnd(int k, int N, int m){
    return (k+1) * ceil(N/m);
}

void *thread_operations(void *args){
    Thread *thread = args;
    char message[100];

    sprintf(message, "thread #%d starting work", thread->id);
    printOper("START", message);

    Area *area = thread->area;

    printf("%d %d %d %d\n", area->y_start, area->y_end, area->x_start, area->x_end);

    for(int row=area->y_start; row<area->y_end; row++){
        for(int col=area->x_start; col<area->x_end; col++){
            int position = row * image->width + col;
            // printf("%d\n", position);

            // printf("thread#%d:\tfrom: %d\tto:%d\n",
            //     thread->id,
            //     image->colors[position],
            //     image->maxColor - image->colors[position]);

            image->colors[position] = image->maxColor - image->colors[position];
        }
    }

    sprintf(message, "thread #%d ended work", thread->id);
    printOper("END", message);

    free(thread);

    return RETURN_SUCCESS;
}

pthread_t createThread(int i, const char *mode){
    pthread_t thread_id;
    Thread *thread = calloc(1, sizeof(Thread));
    
    thread->id = i;

    if(strcmp("numbers", mode) == 0){
        // place for spliting colors for number's mode
    }

    if(strcmp("blocks", mode) == 0){
        int x_end;
        int x_start = getAreaStart(i, image->width, thread_amount);

        if(i == thread_amount -1 ) x_end = image->width;
        else x_end = getAreaEnd(i, image->width, thread_amount);

        thread->area = calloc(1, sizeof(Area));
        thread->area->x_start = x_start;
        thread->area->x_end = x_end;
        thread->area->y_start = 0;
        thread->area->y_end = image->height; 

        printf("\tid: %d\tstart: %d\tend: %d\n",
            thread->id,
            thread->area->x_start ,
            thread->area->x_end);
    }

    if(pthread_create(&thread_id, NULL, thread_operations, thread) != 0){
        error("INCORRECT_ARGUMENTS", "[threads-amount] [mode] [image-input-path] [image-output-file]");
        exit(1);
    }

    return thread_id;
}

Image *getImage(const char* path){
    Image *image = calloc(1, sizeof(Image));
    FILE *file;
    size_t buffer_size = 128;
    char *line = calloc(buffer_size, sizeof(char));

    if((file = fopen(path, "r")) == NULL){
        error("COULDNT_OPEN_FILE", "program occured problem with opening file");
        exit(1);
    }

    fscanf(file, "%s", line);
    if(strcmp(line, "P2") != 0){
        error("INVALID_FILE_FORMAT", "file from provided path is incorrect, becouse it doesn't start with 'P2' header");
        exit(1);
    }

    fscanf(file, "%d", &image->width);
    fscanf(file, "%d", &image->height);
    fscanf(file, "%d", &image->maxColor);
    image->amount = image->width * image->height;

    image->colors = calloc(
        image->amount,
        sizeof(unsigned char)
    );

    for(int col=0; col<image->height; col++){
        for(int row=0; row<image->width; row++){
            int color;
            fscanf(file, "%d", &color);

            image->colors[col * image->width + row] = (unsigned char) color;
        }
    }
    
    fclose(file);

    return image;
}

void saveImage(const char *path){
    FILE *file;

    if((file = fopen(path, "w")) == NULL){
        error("COULDNT_OPEN_FILE", "program occured problem with creating file");
        exit(1);
    }

    printInfo("SAVE", "main thread starts to save image");

    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", image->width, image->height);
    fprintf(file, "%d\n", image->maxColor);

    for(int i=0; i<image->amount; i++){
        fprintf(file, "%d ", image->colors[i]);

        if(i % COLS_IN_FILE == COLS_IN_FILE-1) fprintf(file, "\n");
    }

    printInfo("SAVE", "main thread successfully saved image");

    fclose(file);
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
    thread_amount = atoi(argv[1]);
    mode = argv[2];
    img_input = argv[3];
    img_output = argv[4];

    // Loading image
    image = getImage(img_input);

    threads = calloc(thread_amount, sizeof(pthread_t));

    printInfo("INITIALIZATION", "threads are splitted among these areas");

    for(int i=0; i<thread_amount; i++){
        threads[i] = createThread(i, mode);
    }

    for(int i=0; i<thread_amount; i++){
        int result;

        pthread_join(threads[i], (void **) &result);
    }

    saveImage(img_output);

    free(image);
    free(threads);

    return RETURN_SUCCESS;
}