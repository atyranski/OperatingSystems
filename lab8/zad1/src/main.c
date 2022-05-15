#include "configs.h"

int thread_amount;
const char *mode;
const char *img_input;
const char *img_output; 
Image *image;
pthread_t *threads;
int *border_values;

double getTimeElapsed(struct timespec *start, struct timespec *end){
    return ((end->tv_sec - start->tv_sec) * 10e6 + (end->tv_nsec - start->tv_nsec) / 10e3) / 10e2;
}

int getAreaStart(int k, int N, int m){
    return k * ceil(N/m);
}

int getAreaEnd(int k, int N, int m){
    return (k+1) * ceil(N/m);
}

void calculateOccurances(int *occurances){
    for(int i=0; i<image->amount; i++){
        occurances[image->colors[i]]++;
    }
}

void calculateBorderValues(){
    int *occurances = calloc(image->maxColor + 1, sizeof(int));
    border_values = calloc(thread_amount, sizeof(int));

    int points_per_thread = image->amount / thread_amount;
    printf("expected points per thread: %d\n", points_per_thread); 
    calculateOccurances(occurances);


    int value = 0;
    int index = 0;
    for(int i=0; i<thread_amount; i++){
        while(value < (i+1) * points_per_thread && index < image->maxColor + 1){
            index++;
            value += occurances[index];
        }
        border_values[i] = index;
    }

    for(int i=0; i<thread_amount; i++){
        printf("thread #%d is responsible for range to: %d\n", i, border_values[i]);
    }

    free(occurances);
}

bool isValueBetween(int x, int left, int right){
    if(x >= left && x <= right) return true;
    else false;
}

void *numbers_operations(void *args){
    Thread_numbers *thread = args;
    char message[100];
    double *time_result = calloc(1, sizeof(time_result));

    sprintf(message, "thread #%d starting work", thread->id);
    printOper("START", message);

    struct timespec start, end;
    if(clock_gettime(CLOCK_REALTIME, &start) < 0){
        error("COULDNT_GET_TIME", "program occured problemy with getting time from timespec");
        exit(1);
    }

    for(int row=0; row<image->height; row++){
        for(int col=0; col<image->width; col++){
            int position = row * image->width + col;

            if(isValueBetween(
                image->colors[position],
                thread->range_from,
                thread->range_to)
            ) image->colors[position] = image->maxColor - image->colors[position];
        }
    }

    if(clock_gettime(CLOCK_REALTIME, &end) == -1){
        error("COULDNT_GET_TIME", "program occured problemy with getting time from timespec");
        exit(1);
    }

    sprintf(message, "thread #%d ended work", thread->id);
    printOper("END", message);

    free(thread);

    *time_result = getTimeElapsed(&start, &end);

    pthread_exit(time_result);
}

void *blocks_operations(void *args){
    Thread_blocks *thread = args;
    char message[100];
    double *time_result = calloc(1, sizeof(time_result));

    sprintf(message, "thread #%d starting work", thread->id);
    printOper("START", message);

    Area *area = thread->area;

    struct timespec start, end;
    if(clock_gettime(CLOCK_REALTIME, &start) < 0){
        error("COULDNT_GET_TIME", "program occured problemy with getting time from timespec");
        exit(1);
    }

    for(int row=area->y_start; row<area->y_end; row++){
        for(int col=area->x_start; col<area->x_end; col++){
            int position = row * image->width + col;

            image->colors[position] = image->maxColor - image->colors[position];
        }
    }

    // sleep(1);

    if(clock_gettime(CLOCK_REALTIME, &end) == -1){
        error("COULDNT_GET_TIME", "program occured problemy with getting time from timespec");
        exit(1);
    }

    sprintf(message, "thread #%d ended work", thread->id);
    printOper("END", message);

    free(thread);

    *time_result = getTimeElapsed(&start, &end);

    pthread_exit(time_result);
}

pthread_t createThread(int i, const char *mode){
    pthread_t thread_id;

    if(strcmp("numbers", mode) == 0){
        Thread_numbers *thread = calloc(1, sizeof(Thread_numbers));
        thread->id = i;

        if(i == 0){
            thread->range_from = 0;
            thread->range_to = border_values[i];
        } else {
            thread->range_from= border_values[i-1] + 1;
            thread->range_to = border_values[i];
        }

        if(pthread_create(&thread_id, NULL, numbers_operations, thread) != 0){
            error("INCORRECT_ARGUMENTS", "[threads-amount] [mode] [image-input-path] [image-output-file]");
            exit(1);
        }

    }

    if(strcmp("blocks", mode) == 0){
        int x_end;
        int x_start = getAreaStart(i, image->width, thread_amount);
        Thread_blocks *thread = calloc(1, sizeof(Thread_blocks));
        thread->id = i;

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

        if(pthread_create(&thread_id, NULL, blocks_operations, thread) != 0){
            error("INCORRECT_ARGUMENTS", "[threads-amount] [mode] [image-input-path] [image-output-file]");
            exit(1);
        }
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
    threads = calloc(thread_amount, sizeof(pthread_t));
    double *time_results = calloc(thread_amount, sizeof(double));
    
    // getting ending time of all operations
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    // Loading image
    image = getImage(img_input);

    printInfo("INITIALIZATION", "threads are splitted among these areas");

    // if mode is 'numbers' we need to split range of all points into almost
    // even ranges
    if(strcmp("numbers", mode) == 0) calculateBorderValues();

    // createing threads
    for(int i=0; i<thread_amount; i++) threads[i] = createThread(i, mode);

    // waiting for all threads to end
    for(int i=0; i<thread_amount; i++){
        double *result;

        pthread_join(threads[i], (void **) &result);
        time_results[i] = *result;
    }

    // getting ending time of all operations
    clock_gettime(CLOCK_REALTIME, &end);

    // printing summary of time results
    printf("\n\n============ SUMMARY =============\n");
    printf("Total time elapsed:\t%lfms\n", getTimeElapsed(&start, &end));
    for(int i=0; i<thread_amount; i++) printf("\t[#%d]\t%lfms\n", i, time_results[i]);

    // saving object Image to file
    saveImage(img_output);

    // free memory 
    if(strcmp("numbers", mode) == 0) free(border_values);
    free(image);
    free(threads);
    free(time_results);

    return RETURN_SUCCESS;
}