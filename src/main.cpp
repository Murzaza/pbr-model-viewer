#include "App.hpp"

int main(int argc, char **argv)
{
    // Create Renderer
    App app(1600, 1200, "PBR VIEWER");

    //Run Render Loop
    app.start();

    return EXIT_SUCCESS;
}