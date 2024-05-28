
#include "App.h"
#include "LiveCodeApp.cpp"
class MyApp : public App {
public:
    std::shared_ptr<App> liveCodeApp;
	MyApp(Graphics &g)
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

	auto app = std::make_shared<MyApp>(g);

	return app;
}
