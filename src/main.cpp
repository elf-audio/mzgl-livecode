
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

	void update() override {
		dylib.update();
		if (liveCodeApp) liveCodeApp->update();
	}
	void draw() override {
		if (liveCodeApp) liveCodeApp->draw();
	}

	bool openUrl(ScopedUrlRef url) override {
		if (liveCodeApp) return liveCodeApp->openUrl(url);
		return false;
	}

	// FILE DROP
	void fileDragUpdate(float x, float y, int touchId, int numFiles) override {
		if (liveCodeApp) liveCodeApp->fileDragUpdate(x, y, touchId, numFiles);
	}

	// return true to accept, false to reject
	void filesDropped(const std::vector<ScopedUrlRef> &paths, int touchId) override {
		if (liveCodeApp) liveCodeApp->filesDropped(paths, touchId);
	}

	void fileDragCancelled(int touchId) override {
		if (liveCodeApp) liveCodeApp->fileDragCancelled(touchId);
	}
	void fileDragExited(float x, float y, int id) override {
		if (liveCodeApp) liveCodeApp->fileDragExited(x, y, id);
	}

	// return true if you can open, false if you can't
	bool canOpenFiles(const std::vector<std::string> &paths) override {
		if (liveCodeApp) return liveCodeApp->canOpenFiles(paths);
		return false;
	}

	void drawAfterUI() override {
		if (liveCodeApp) liveCodeApp->drawAfterUI();
	}
	void exit() override {
		if (liveCodeApp) liveCodeApp->exit();
	}

	void resized() override {
		if (liveCodeApp) liveCodeApp->resized();
	}

	void mouseScrolled(float x, float y, float dX, float dY) override {
		if (liveCodeApp) liveCodeApp->mouseScrolled(x, y, dX, dY);
	}
	void mouseZoomed(float x, float y, float zoom) override {
		if (liveCodeApp) liveCodeApp->mouseZoomed(x, y, zoom);
	}

	void touchOver(float x, float y) override {
		if (liveCodeApp) liveCodeApp->touchOver(x, y);
	}
	void touchDown(float x, float y, int id) override {
		if (liveCodeApp) liveCodeApp->touchDown(x, y, id);
	}
	void touchMoved(float x, float y, int id) override {
		if (liveCodeApp) liveCodeApp->touchMoved(x, y, id);
	}
	void touchUp(float x, float y, int id) override {
		if (liveCodeApp) liveCodeApp->touchUp(x, y, id);
	}

	void keyDown(int key) override {
		if (liveCodeApp) liveCodeApp->keyDown(key);
	}
	void keyUp(int key) override {
		if (liveCodeApp) liveCodeApp->keyUp(key);
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