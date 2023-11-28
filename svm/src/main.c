#include <SDL2/SDL.h>

#include <kazamori.h>
#include <print.h>


#define DEBUG

#define title "SVM"
#define window_width (16 * 65)
#define window_height (9 * 65)


int init();
void quit();
void set_color(SDL_Color* color);
SDL_Color* create_color(SDL_Color* color, int r, int g, int b, int a);
void draw_circle(double x, double y, double radius);
void draw_type_border(int border_size);


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int surface[window_width][window_height];


int main(int argc, char* argv[]) {
    if (init() != 0) {
        printf("!!! ERROR in init() !!!\n");
        return 1;
    }


    for (int xi = 0; xi < window_width; ++xi) {
        for (int yi = 0; yi < window_height; ++yi) {
            surface[xi][yi] = 0;
        }
    }


    int types_count = 2;
    SDL_Color** colors = (SDL_Color**)malloc(sizeof(SDL_Color*) * types_count);
    colors[0] = create_color(colors[0], 220, 40, 55, 255);
    colors[1] = create_color(colors[1], 50, 50, 220, 255);

    SDL_Color** background_colors = (SDL_Color**)malloc(sizeof(SDL_Color*) * types_count);
    background_colors[0] = create_color(background_colors[0], 255, 180, 180, 255);
    background_colors[1] = create_color(background_colors[1], 180, 180, 255, 255);

    SDL_Color* border_color = (SDL_Color*)malloc(sizeof(SDL_Color));
    border_color = create_color(border_color, 45, 45, 45, 255);

    SDL_Color* background_color = (SDL_Color*)malloc(sizeof(SDL_Color));
    background_color = create_color(background_color, 255, 255, 255, 255);

    DataSet* train_set = DataSet_construct(0, 2, types_count);

    FullConnected* layer = FullConnected_construct(3, (int[]){2, 4, types_count});
    FullConnected_train_construct(layer, 0.6, 0.4);


    int x, y;
    int type = 0;

    bool run = true;
    bool action = true;
    SDL_Event event;

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
                if (event.key.keysym.sym == SDLK_1) {
                    action = true;
                    type -= 1;
                    if (type < 0) {
                        type = types_count - 1;
                    }
                }
                if (event.key.keysym.sym == SDLK_2) {
                    action = true;
                    type += 1;
                    if (type >= types_count) {
                        type = 0;
                    }
                }
                if (event.key.keysym.sym == SDLK_q) {
                    action = true;
                    printf("epochs %d\n", FullConnected_train_alpha(layer, train_set, 0.0003));
                    FullConnected_check(layer, train_set);
                    for (int xi = 0; xi < window_width; ++xi) {
                        for (int yi = 0; yi < window_height; ++yi) {
                            double inp[] = {(double)xi / window_width, (double)yi / window_height};
                            double* ptr = inp;
                            FullConnected_forward(layer, ptr);
                            for (int i = 0; i < types_count; ++i) {
                                if (layer->x[layer->length - 1][0] > 0.5) {
                                    surface[xi][yi] = 1;
                                } else {
                                    surface[xi][yi] = 2;
                                }
                            }
                        }
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    action = true;
                    SDL_GetMouseState(&x, &y);
                    if (type == 0) {
                        DataSet_add(train_set, (double[]){(double)x / window_width, (double)y / window_height, 1, 0});
                    } else {
                        DataSet_add(train_set, (double[]){(double)x / window_width, (double)y / window_height, 0, 1});
                    }
                    DataSet_print(train_set);
                }
            }
        }

        if (action) {
            action = false;
            for (int xi = 0; xi < window_width; ++xi) {
                for (int yi = 0; yi < window_height; ++yi) {
                    if (surface[xi][yi] == 1) {
                        set_color(background_colors[0]);
                    } else if (surface[xi][yi] == 2) {
                        set_color(background_colors[1]);
                    } else {
                        set_color(background_color);
                    } 
                    SDL_RenderDrawPoint(renderer, xi, yi);
                }
            }

            for (int i = 0; i < train_set->length; ++i) {
                set_color(border_color);
                draw_circle(
                    (int)(window_width * train_set->input[i][0]),
                    (int)(window_height * train_set->input[i][1]),
                    15
                );
                set_color(colors[(int)train_set->output[i][1]]);
                draw_circle(
                    (int)(window_width * train_set->input[i][0]),
                    (int)(window_height * train_set->input[i][1]),
                    15 - 2
                );
            }

            set_color(colors[type]);
            draw_type_border(5);

            SDL_RenderPresent(renderer);
        }
    }

    for (int i = 0; i < types_count; ++i) {
        free(colors[i]);
    }
    free(colors);
    
    for (int i = 0; i < types_count; ++i) {
        free(background_colors[i]);
    }
    free(background_colors);

    free(background_color);
    free(border_color);

    FullConnected_train_destruct(layer);
    FullConnected_destruct(layer);

    DataSet_destruct(train_set);

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


void set_color(SDL_Color* color) {
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
}


SDL_Color* create_color(SDL_Color* color, int r, int g, int b, int a) {
    color = (SDL_Color*)malloc(sizeof(SDL_Color));
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
    return color;
}


void draw_circle(double x, double y, double radius) {
    for (int xi = x - radius; xi <= x + radius; ++xi) {
        for (int yi = y - radius; yi <= y + radius; ++yi) {
            if ( (xi - x) * (xi - x) + (yi - y) * (yi - y) <= radius * radius ) {
                SDL_RenderDrawPoint(renderer, xi, yi);
            }
        }
    }
}


void draw_type_border(int border_size) {
    SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, window_width, border_size});
    SDL_RenderFillRect(renderer, &(SDL_Rect){0, window_height - border_size, window_width, border_size});
    SDL_RenderFillRect(renderer, &(SDL_Rect){0, border_size, border_size, window_width - 2 * border_size});
    SDL_RenderFillRect(renderer, &(SDL_Rect){window_width - border_size, border_size, border_size, window_width - 2 * border_size});
}
