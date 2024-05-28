
#include "App.h"
// #include "myApp.cpp"
#include "RecompilingAppDylib.h"
#include "EventDispatcher.h"
class LiveCodeReloadingApp : public App {
public:
	RecompilingAppDylib dylib;
	std::shared_ptr<EventDispatcher> eventDispatcher;
	std::shared_ptr<AudioSystem> audioSystem;

	LiveCodeReloadingApp(Graphics &g)
		: App(g)
		, dylib(g) {
		audioSystem = std::make_shared<AudioSystem>();
		audioSystem->setBufferSize(64);
		audioSystem->setup(2, 2);
		audioSystem->bindToApp(this);
		audioSystem->start();
		dylib.willCloseDylib = [&]() {
			if (eventDispatcher) eventDispatcher->exit();
			eventDispatcher = nullptr;
		};

		dylib.recompiled = [&](std::shared_ptr<App> app) {
			eventDispatcher = std::make_shared<EventDispatcher>(app);
			eventDispatcher->setup();
			eventDispatcher->resized();
		};
		dylib.setup("/Users/marek/mzgl-livecode/src/MyApp.h");
	}

	void audioIn(float *data, int frames, int chans) override {
		if (dylib.tryLock()) {
			if (eventDispatcher) eventDispatcher->app->audioIn(data, frames, chans);
			dylib.unlock();
		}
	}
	void audioOut(float *data, int frames, int chans) override {
		if (dylib.tryLock()) {
			if (eventDispatcher) eventDispatcher->app->audioOut(data, frames, chans);
			dylib.unlock();
		}
	}

	void update() override {
		dylib.update();
		if (eventDispatcher) eventDispatcher->update();
	}
	void draw() override {
		if (eventDispatcher) eventDispatcher->draw();
	}

	bool openUrl(ScopedUrlRef url) override {
		if (eventDispatcher) return eventDispatcher->openUrl(url);
		return false;
	}

	// FILE DROP
	void fileDragUpdate(float x, float y, int touchId, int numFiles) override {
		if (eventDispatcher) eventDispatcher->fileDragUpdate(x, y, touchId, numFiles);
	}

	// return true to accept, false to reject
	void filesDropped(const std::vector<ScopedUrlRef> &paths, int touchId) override {
		if (eventDispatcher) eventDispatcher->filesDropped(paths, touchId);
	}

	void fileDragCancelled(int touchId) override {
		if (eventDispatcher) eventDispatcher->fileDragCancelled(touchId);
	}
	void fileDragExited(float x, float y, int id) override {
		if (eventDispatcher) eventDispatcher->fileDragExited(x, y, id);
	}

	// return true if you can open, false if you can't
	bool canOpenFiles(const std::vector<std::string> &paths) override {
		if (eventDispatcher) return eventDispatcher->canOpenFiles(paths);
		return false;
	}

	//	void drawAfterUI() override {
	//		if (eventDispatcher) eventDispatcher->drawAfterUI();
	//	}
	void exit() override {
		if (eventDispatcher) eventDispatcher->exit();
	}

	void resized() override {
		if (eventDispatcher) eventDispatcher->resized();
	}

	void mouseScrolled(float x, float y, float dX, float dY) override {
		if (eventDispatcher) eventDispatcher->mouseScrolled(x, y, dX, dY);
	}
	void mouseZoomed(float x, float y, float zoom) override {
		if (eventDispatcher) eventDispatcher->mouseZoomed(x, y, zoom);
	}

	void touchOver(float x, float y) override {
		if (eventDispatcher) eventDispatcher->touchOver(x, y);
	}
	void touchDown(float x, float y, int id) override {
		if (eventDispatcher) eventDispatcher->touchDown(x, y, id);
	}
	void touchMoved(float x, float y, int id) override {
		if (eventDispatcher) eventDispatcher->touchMoved(x, y, id);
	}
	void touchUp(float x, float y, int id) override {
		if (eventDispatcher) eventDispatcher->touchUp(x, y, id);
	}

	void keyDown(int key) override {
		if (eventDispatcher) eventDispatcher->keyDown(key);
	}
	void keyUp(int key) override {
		if (eventDispatcher) eventDispatcher->keyUp(key);
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