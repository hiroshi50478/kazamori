#include <stdio.h>
#include <stdbool.h>

// #include <sys/stat.h>
// #include <dirent.h>

// #include <libjpeg.h>

#include <SDL2/SDL.h>

#include <kazamori.h>
// #include <print.h>

#define title "app"

#define window_width (16 * 65)
#define window_height (9 * 65)

#define canvas_width (16 * 45)

#define pixel_width (16 * 5)
#define pixel_height (9 * 5)


int init();
void quit();
void add(ImageSet* image_set, int index)


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int pixels[window_height / pixel_height][canvas_width / pixel_width];


int main(int argc, char* argv[]) {
    if (init() != 0) {
        printf("!!! ERROR in init() !!!\n");
        return 1;
    }
	
    for (int y = 0; y < window_height / pixel_height; ++y) {
        for (int x = 0; x < canvas_width / pixel_width; ++x) {
            pixels[y][x] = 0;
        }
    }

    ImageSet* image_set = ImageSet_construct(0, 3, window_height / pixel_height,canvas_width / pixel_width, 2);
    // ImageSet_load(image_set, "data.txt");

    bool run = true;
    bool action = true;
    SDL_Event event;

    bool click_left = false;
    bool click_right = false;

    while (run) {
        while(SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                run = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    run = false;
                    break;
                }
                if (event.key.keysym.sym == SDLK_0) {
                    add(image_set, 0);
                    ImageSet_save(image_set);
                }
                if (event.key.keysym.sym == SDLK_1) {
                    add(image_set, 1);
                    ImageSet_save(image_set);
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    click_left = true;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    click_right = true;
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    click_left = false;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    click_right = false;
                }
            }
        }

        if (click_left) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (x > 0 && y > 0 && x < canvas_width && y < window_height) {
                pixels[y / pixel_height][x / pixel_width] = 1;
                action = true;
            }
        }

        if (click_right) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (x > 0 && y > 0 && x < canvas_width && y < window_height) {
                pixels[y / pixel_height][x / pixel_width] = 0;
                action = true;
            }
        }


        if (action) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            for (int x = 0; x <= canvas_width; x += pixel_width) {
                SDL_RenderDrawLine(renderer, x, 0, x, window_height);
            }
            for (int y = 0; y <= window_height; y += pixel_height) {
                SDL_RenderDrawLine(renderer, 0, y, canvas_width, y);
            }

            for (int y = 0; y < window_height / pixel_height; ++y) {
                for (int x = 0; x < canvas_width / pixel_width; ++x) {
                    if (pixels[y][x] == 1) {
                        SDL_RenderFillRect(renderer, &(SDL_Rect){
                            x * pixel_width,
                            y * pixel_height,
                            pixel_width,
                            pixel_height
                        });
                    }
                }
            }

            SDL_RenderPresent(renderer);
            action = false;
        }
    }

	quit();

	return 0;
}


int init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("!!! ERROR in SDL_Init() !!!\n%s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(title, 100, 100, window_width, window_height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("!!! ERROR in SDL_CreateWindow() !!!\n%s", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("!!! ERROR in SDL_CreateRenderer() !!!\n%s", SDL_GetError());
        return 1;
    }

    return 0;
}


void quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void add(ImageSet* image_set, int index) {
    double input[image_set->channels * image_set->height * image_set->width];
    for (int c = 0; c < image_set->channels; ++c) {
        for (int h = 0; h < image_set->height; ++h) {
            for (int w = 0; w < image_set->width; ++w) {
                input[c * image_set->height * image_set->width + h * image_set->width + w] = pixels[h][w];
            }
        }
    }
    
    double output[image_set->output_length];
    for (int i = 0; i < image_set->output_length; ++i) {
        output[i] = 0;
    }
    output[index] = 1;

    ImageSet_add(image_set, input, output); 
}

// void save_image(int number) {
//     // char path[20];
//     // sprintf(path, "%dx%d", canvas_width / pixel_width, window_height / pixel_height); 
//     // printf("%s\n", path);
//     FILE* file = fopen('data.txt', 'w');
//     if (file == NULL) {
//         printf("!!! ERROR in file handling for saving !!!\n");
//     }
//     fprintf(file, "%d %d %d ", 3, window_height / pixel_height, canvas_width / pixel_width);

// }

// void save_image(int number) {
    // int width = window_width / pixel_width;
    // int height = window_height / pixel_height;

    // char* scr = (char*)malloc(width * height * 3);

    // for (int i = 0; i < width; ++i) {
    //     for (int j = 0; j < height; ++j) {
    //         char* p = scr + j * width * 3 + i * 3;
    //         *p = i + j;
    //         ++p;
    //         *p = i - j;
    //         ++p;
    //         *p = i * j;
    //     }
    // }

    // struct jpeg_compress_struct cinfo;
    // struct jpeg_error_mgr jerr;
    // cinfo.err = jpeg_std_error(&jerr);
    // jpeg_create_compress(&cinfo);

    // jpeg_stdio_dest(&cinfo, stdout);
    
    // cinfo.image_width = width;
    // cinfo.image_height = height;
    // cinfo.input_components = 3;
    // cinfo.in_color_space = JCS_RGB;

    // jpeg_set_defaults(&cinfo);
    // jpeg_set_quality(&cinfo, 75, true);
    // jpeg_start_compress(&cinfo, TRUE);

    // JSAMPROW row_pointer[1];
    // int row_stride;
    // row_stride = cinfo.image_width * 3;

    // while (cinfo.next_scanline < cinfo.image_height) {
    //     row_pointer[0] = (JSAMPLE*)(scr + cinfo.next_scanline * row_stride);
    //     jpeg_write_scanlines(&cinfo, row_pointer, 1);
    // }

    // jpeg_finish_compress(&cinfo);
    // jpeg_destroy_compress(&cinfo);

    // free(scr);
// }