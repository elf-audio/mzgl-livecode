
#include "App.h"

class LiveCodeApp : public App {
public:
    LiveCodeApp(Graphics &g)
            : App(g) {}
};

class MyApp : public LiveCodeApp {
public:
    MyApp(Graphics &g)
        : LiveCodeApp(g) {}

    void draw() {
        g.setColor(randuf(), randuf(), randuf());
        g.drawCircle(randi(g.width / 2), randi(g.height / 2), g.width / 4);
    }
};

std::shared_ptr<LiveCodeApp> instantiateLiveCodeApp(Graphics &g) {
    g.width = 512;
    g.height = 512;

    auto app = std::make_shared<MyApp>(g);

    return app;
}