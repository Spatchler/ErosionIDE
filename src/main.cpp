#include "UI.hpp"

int main(int argc, char* args[]) {    
    ui::window window = ui::window("Erosion IDE - vALPHA", math::vec2(1280, 720));

    ui::surface mainWorkspace;
    ui::surface fileExplorer;

    window.split({1, 6});

    while (window.running) {
        window.update();
    }

    return 0;
}