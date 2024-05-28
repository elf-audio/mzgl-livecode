
#include "App.h"
#include "myApp.cpp"
class LiveCodeReloadingApp : public App {
public:
    std::shared_ptr<App> liveCodeApp;
    LiveCodeReloadingApp(Graphics &g)
		: App(g) {
        liveCodeApp = std::make_shared<LiveCodeApp>(g);
    }

	void draw() {
        liveCodeApp->draw();
	}
};

std::shared_ptr<App> instantiateApp(Graphics &g) {
	g.width = 512;
	g.height = 512;

	auto app = std::make_shared<LiveCodeReloadingApp>(g);

	return app;
}
