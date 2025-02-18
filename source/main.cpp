
#include "SSBApp.h"

using namespace cugl;


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
    app.setOrganization("GDIAC");
    
    //app.setFullscreen(true);
    app.setDisplaySize(1024, 576);
    app.setFPS(60.0f);
    
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
