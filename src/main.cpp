
#include "App.h"
// #include "myApp.cpp"
#include "RecompilingAppDylib.h"
class LiveCodeReloadingApp : public App {
public:
	App *liveCodeApp = nullptr;
	RecompilingAppDylib dylib;
	LiveCodeReloadingApp(Graphics &g)
		: App(g)
		, dylib(g) {
		dylib.recompiled = [&](void *dlib) { liveCodeApp = static_cast<App *>(dlib); };
		dylib.setup("/Users/marek/mzgl-livecode/src/MyApp.h");
	}
	void update() override { dylib.update(); }
	void draw() override {
		if (liveCodeApp) liveCodeApp->draw();
	}
};

std::shared_ptr<App> instantiateApp(Graphics &g) {
	g.width	 = 512;
	g.height = 512;

	auto app = std::make_shared<LiveCodeReloadingApp>(g);

	return app;
}
#include "PluginEditor.h"
std::shared_ptr<PluginEditor> instantiatePluginEditor(Graphics &g, std::shared_ptr<Plugin> plugin) {
	return nullptr;
}

std::shared_ptr<Plugin> instantiatePlugin() {
	return nullptr;
}

#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#include "dr_flac.h"
#include "dr_mp3.h"
#include "dr_wav.h"