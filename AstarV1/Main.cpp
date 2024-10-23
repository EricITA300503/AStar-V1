#include <stdio.h>
#include <SDL.h>
#include <cstdlib>
#include <math.h>
#include <iostream> 

// Node structure representing a cell in the grid
struct Node {
    int solid = 0;           // Indicates if the node is solid (obstacle)
    int end_goal = 0;       // Flag for the end goal node
    int start_point = 0;    // Flag for the starting point node
    int x = 0;              // X-coordinate of the node
    int y = 0;              // Y-coordinate of the node
    int g = 0;              // Cost from start to this node
    int h = 0;              // Heuristic cost to the end goal
    int f = 0;              // Total cost (g + h)
    int closed = 0;         // Indicates if the node has been evaluated
    int path = 0;           // Flag for the final path

    Node* parent = nullptr; // Pointer to the parent node

    int GetClosed() {
        return closed;      // Return the closed status of the node
    }
};

//Grid Dimension
int grid_width = 60;
int grid_height = 40;

// Array to store all nodes in the grid
struct Node gridPoint[60][40];

int grid_cell_size = 20;


// + 1 so that last grid line fit the screen
int window_width = (grid_width * grid_cell_size) + 1;
int window_height = (grid_height * grid_cell_size) + 1;

//Coordinates for my start and end points
int startingpointx = 1;
int startingpointy = 1;
int endpointx = 58;
int endpointy = 38;

//Creating the cells to represent my points
SDL_Rect starting_point = { startingpointx * grid_cell_size, startingpointy * grid_cell_size, grid_cell_size, grid_cell_size };
SDL_Rect end_goal = { endpointx * grid_cell_size,endpointy * grid_cell_size, grid_cell_size, grid_cell_size };



// Function to calculate the Euclidean distance between two points
double distance2D(int x1, int y1, int x2, int y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

//Calculate g, h, f of one node
void calculateNode(Node* node, int x, int y) {
    
    // Skip solid nodes and closed nodes
    if (node->solid || gridPoint[x][y].closed) {
        return;
    }

    // Compute heuristic and cost values
    gridPoint[x][y].h = distance2D(gridPoint[x][y].x, gridPoint[x][y].y, end_goal.x, end_goal.y) * 10;

    gridPoint[x][y].g = distance2D(gridPoint[x][y].x, gridPoint[x][y].y, startingpointx, startingpointy) * 10;

    gridPoint[x][y].f = gridPoint[x][y].g + gridPoint[x][y].h;
    
    // Update the parent node if this path is shorter
    if (gridPoint[x][y].parent != nullptr) {
        if (node->f < gridPoint[x][y].parent->f && node->h < gridPoint[x][y].parent->h) {
            gridPoint[x][y].parent = node;
        }
    }
    else {
        gridPoint[x][y].parent = node; // Set parent if none exists
    }



}

// Calculate the neighboring nodes for a given node
void calculateNeighbours(int x, int y) {

    // Check neighboring cells (left, right, up, down)
    if (x > 0) {
        calculateNode(&gridPoint[x][y], x - 1, y); // Left
    }
    if (x + 1 < grid_width) {
        calculateNode(&gridPoint[x][y], x + 1, y); // Right
    }
    if (y > 0) {
        calculateNode(&gridPoint[x][y], x, y - 1); // Up
    }
    if (y + 1 < grid_height) {
        calculateNode(&gridPoint[x][y], x, y + 1); // Down
    }

    // Check diagonal neighbors
    if (x > 0 && y > 0) {
        calculateNode(&gridPoint[x][y], x - 1, y - 1); // Top-left
    }
    if (x + 1 < grid_width && y + 1 < grid_height) {
        calculateNode(&gridPoint[x][y], x + 1, y + 1); // Bottom-right
    }
    if (x + 1 < grid_width && y > 0) {
        calculateNode(&gridPoint[x][y], x + 1, y - 1); // Top-right
    }
    if (x > 0 && y + 1 < grid_height) {
        calculateNode(&gridPoint[x][y], x - 1, y + 1); // Bottom-left
    }
}

// Retrieve the shortest path from the endpoint to the starting point
void GetPath(int x, int y) {
    if (x == startingpointx && y == startingpointy)
        return;
    if (gridPoint[x][y].parent->parent == &gridPoint[x][y])
        return;
    gridPoint[x][y].path = true;
    GetPath(gridPoint[x][y].parent->x, gridPoint[x][y].parent->y);
}

int main(int argc, char* args[]) {

    // Initialize the grid cursor object
    SDL_Rect grid_cursor = { grid_cell_size };


    // Define colors for rendering
    SDL_Color grid_background = { 10,9,8,255 };
    SDL_Color grid_line_color = { 34,51,59, 255 };
    SDL_Color grid_cursor_color = { 234, 224, 213, 255 };
    SDL_Color startin_point_color = { 0, 0, 255, 255 };
    SDL_Color end_goal_color = { 255, 165, 0, 255 };
    SDL_Color closed_color = { 255, 0, 0, 255 };
    SDL_Color neighbour_color = { 0, 255, 0, 255 };



    //Initialize nodes
    for (int y = 0; y < grid_height; y++) {
        for (int x = 0; x < grid_width; x++) {
            struct Node newNode;

            newNode.x = x;
            newNode.y = y;

            newNode.h = 0;

            newNode.g = 0;

            newNode.f = 0;
            newNode.closed = 0;
            gridPoint[x][y] = newNode;

        }
    }

 
    // Initialize starting point's cost values
    gridPoint[startingpointx][startingpointy].h = distance2D(gridPoint[startingpointx][startingpointy].x, gridPoint[startingpointx][startingpointy].y, end_goal.x, end_goal.y) * 10;

    gridPoint[startingpointx][startingpointy].g = distance2D(gridPoint[startingpointx][startingpointy].x, gridPoint[startingpointx][startingpointy].y, startingpointx, startingpointy) * 10;

    gridPoint[startingpointx][startingpointy].f = gridPoint[startingpointx][startingpointy].g + gridPoint[startingpointx][startingpointy].h;
   
    //Initializes the starting point's parent property to itself
    gridPoint[startingpointx][startingpointy].parent = &gridPoint[startingpointx][startingpointy];


    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window;
    SDL_Renderer* renderer;

    // Create SDL window and renderer
    if (SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Create window and renderer: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetWindowTitle(window, "A* grid");

    SDL_bool quit = SDL_FALSE;
    SDL_bool mouse_active = SDL_FALSE;

    bool start_solving = false;

    // Main program loop to handle events and render the grid
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            int cell_x = event.motion.x / grid_cell_size;
            int cell_y = event.motion.y / grid_cell_size;

            switch (event.type) {
                // Handle mouse button down events to toggle cell states
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_active = SDL_TRUE;
                    // Toggle the solid state of the cell based on its current state
                    if (cell_x >= 0 && cell_x < grid_width && cell_y >= 0 && cell_y < grid_height) {
                        gridPoint[cell_x][cell_y].solid = !gridPoint[cell_x][cell_y].solid;
                    }
                }
                break;
                // Handle mouse button up events to deactivate mouse interaction
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouse_active = SDL_FALSE;
                }
                break;

                // Handle mouse motion events to highlight cells while dragging
            case SDL_MOUSEMOTION:
                if (mouse_active) {
                    if (cell_x >= 0 && cell_x < grid_width && cell_y >= 0 && cell_y < grid_height) {
                        gridPoint[cell_x][cell_y].solid = true;
                    }
                }
                break;
                // Start the pathfinding algorithm when a key is pressed
            case SDL_KEYDOWN:
                start_solving = true;
                break;
                // Exit the program when the window is closed
            case SDL_QUIT:
                quit = SDL_TRUE;
                break;


            }
        }

        // Execute the A* algorithm to find the shortest path if solving has started
        if (start_solving) {
            struct Node* node = nullptr;

            gridPoint[endpointx][endpointy].f = 0;

            // Search for the next node to process
            for (int y = 0; y < grid_height; y++) {
                for (int x = 0; x < grid_width; x++) {
                    // Find the first open node or the node with the lowest f-value
                    if (node == nullptr && !gridPoint[x][y].GetClosed() && gridPoint[x][y].parent != nullptr) {
                        node = &gridPoint[x][y];
                    }
                    if (node != nullptr) {
                        if (gridPoint[x][y].f < node->f && gridPoint[x][y].h < node->h && gridPoint[x][y].parent != nullptr && !gridPoint[x][y].GetClosed()) {
                            node = &gridPoint[x][y];
                        }
                    }


                }
            }
            // If the endpoint node has been found
            if (node->x == endpointx && node->y == endpointy) {
                start_solving = false;
                gridPoint[endpointx][endpointy].closed = 1;
                GetPath(endpointx, endpointy);
            }
            else if (node != nullptr) {
                
                node->closed = 1;
                calculateNeighbours(node->x, node->y);
            }


        }


        // Render grid background.
        SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g,
            grid_background.b, grid_background.a);
        SDL_RenderClear(renderer);

        // Draw grid lines.
        SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
            grid_line_color.b, grid_line_color.a);


        for (int x = 0; x < 1 + grid_width * grid_cell_size;
            x += grid_cell_size) {
            SDL_RenderDrawLine(renderer, x, 0, x, window_height);
        }

        for (int y = 0; y < 1 + grid_height * grid_cell_size;
            y += grid_cell_size) {
            SDL_RenderDrawLine(renderer, 0, y, window_width, y);
        }



        // Draw grid cells based on their state
        for (int x = 0; x < grid_width; ++x) {
            for (int y = 0; y < grid_height; ++y) {
                if (gridPoint[x][y].solid) {
                    SDL_Rect highlight_rect = { x * grid_cell_size, y * grid_cell_size, grid_cell_size, grid_cell_size };
                    SDL_SetRenderDrawColor(renderer, grid_cursor_color.r, grid_cursor_color.g, grid_cursor_color.b, grid_cursor_color.a);
                    SDL_RenderFillRect(renderer, &highlight_rect);
                }
                else if (gridPoint[x][y].path) {
                    SDL_Rect highlight_rect = { x * grid_cell_size, y * grid_cell_size, grid_cell_size, grid_cell_size };
                    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &highlight_rect);
                }
                else if (gridPoint[x][y].GetClosed()) {
                    SDL_Rect highlight_rect = { x * grid_cell_size, y * grid_cell_size, grid_cell_size, grid_cell_size };
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &highlight_rect);
                }
                else if (gridPoint[x][y].parent != nullptr) {
                    SDL_Rect highlight_rect = { x * grid_cell_size, y * grid_cell_size, grid_cell_size, grid_cell_size };
                    SDL_SetRenderDrawColor(renderer, neighbour_color.r, neighbour_color.g, neighbour_color.b, neighbour_color.a);
                    SDL_RenderFillRect(renderer, &highlight_rect);
                }

            }
        }

        //Drawing my starting point
        SDL_SetRenderDrawColor(renderer, startin_point_color.r, startin_point_color.g, startin_point_color.b, startin_point_color.a);
        SDL_RenderFillRect(renderer, &starting_point);

        //Drawing my goal point
        SDL_SetRenderDrawColor(renderer, end_goal_color.r, end_goal_color.g, end_goal_color.b, 100);
        SDL_RenderFillRect(renderer, &end_goal);

        // Draw grid cursor.
        SDL_SetRenderDrawColor(renderer, grid_cursor_color.r,
            grid_cursor_color.g, grid_cursor_color.b,
            grid_cursor_color.a);
        SDL_RenderFillRect(renderer, &grid_cursor);

        SDL_RenderPresent(renderer);
    }

    //Cleanup and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}