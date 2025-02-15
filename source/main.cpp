
#include "SSBApp.h"

// This keeps us from having to write cugl:: all the time
using namespace cugl;

// These really only apply if the game is windowed (not on mobile device)
#define GAME_WIDTH 1024
#define GAME_HEIGHT 576

/**
 * The main entry point of any CUGL application.
 *
 * This class creates the application and runs it until done.  You may
 * need to tailor this to your application, particularly the application
 * settings.  However, never modify anything below the line marked.
 *
 * @return the exit status of the application
 */
int main(int argc, char * argv[]) {
    // Change this to your application class
    SSBApp app;
    
    /// SET YOUR APPLICATION PROPERTIES
    
    // The unique application name
    app.setName("Sweet Sweet Betrayal");
    
    // The name of your studio (for organizing save files)
    app.setOrganization("GDIAC");

    // Set the window properties (Only applies to OS X/Windows Desktop)
    app.setDisplaySize(GAME_WIDTH, GAME_HEIGHT);
    app.setFPS(60.0f);
    app.setHighDPI(true);
    
    /// DO NOT MODIFY ANYTHING BELOW THIS LINE
    if (!app.init()) {
        return 1;
    }
    
    // Run the application until completion
    app.onStartup();
    while (app.step());
    app.onShutdown();
    
    exit(0);    // Necessary to quit on mobile devices
    return 0;   // This line is never reached
}
