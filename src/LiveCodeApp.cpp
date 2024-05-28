
#include "App.h"

class LiveCodeApp : public App {
public:
    LiveCodeApp(Graphics &g)
        : App(g) {}

    void draw() {
        // g.clear(0, 0, 0);
        g.setColor(randuf(), randuf(), randuf());
        g.drawCircle(randi(g.width / 2), randi(g.height / 2), g.width / 4);
    }
};