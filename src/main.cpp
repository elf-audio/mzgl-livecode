
#include "App.h"
// #include "myApp.cpp"
#include "RecompilingAppDylib.h"
#include "EventDispatcher.h"
#include "AllMidiDevices.h"
#include "readerwriterqueue.h"

std::string appTemplate = "#pragma once\n\n"
						  "#include \"App.h\"\n\n"
						  "class APP_NAME : public App {\n"
						  "public:  APP_NAME(Graphics & g)"
						  "	: App(g) {"
						  "	}\n"
						  "	\n"
						  "	void draw() override {\n"
						  "	}\n"
						  "	\n"
						  "	void audioOut(float *outs, int frames, int chans) override {\n"
						  "	}\n"
						  "};\n";

class LiveCodeReloadingApp
	: public App
	, public MidiListener {
public:
	RecompilingAppDylib dylib;
	std::shared_ptr<EventDispatcher> eventDispatcher;
	std::shared_ptr<AudioSystem> audioSystem;

	void printUsageAndExit(const std::string &exeName) {
		printf("Usage: %s Filename.h [-e] [--mzgl=/path/to/mzgl]\n", exeName.c_str());
		printf("\n\tFilename.h is a file that will be created if it doesn't exist\n"
			   "\n\t-e will open the file in Visual Studio Code(optional)\n"
			   "\n\t--mzgl=/path/to/mzgl will specify the path to the mzgl library \n"
			   "\t\tit needs to be the lib folder inside mzgl \n\n");
		::exit(1);
	}
	AllMidiDevices midi;

	LiveCodeReloadingApp(Graphics &g)
		: App(g)
		, dylib(g) {
		midi.setup();
		midi.addListener(this);
		auto args = getCommandLineArgs();

		std::string mzglRoot	= MZGL_LIBROOT;
		bool shouldOpenInVsCode = false;
		fs::path fileName;
		std::vector<std::string> includes;
		std::string exeName = args[0];
		args.erase(args.begin());

		for (const auto &arg: args) {
			if (arg == "-e") {
				shouldOpenInVsCode = true;
			} else if (arg.find("-I") == 0) {
				includes.push_back(arg.substr(2));
			} else if (arg.rfind("--mzgl=", 0) == 0) {
				mzglRoot = arg.substr(7);
			} else if (arg.size() > 2 && arg.substr(arg.size() - 2) == ".h") {
				fileName = arg;
			} else {
				printf("Unknown argument or invalid file extension: '%s'\n", arg.c_str());
				printUsageAndExit(exeName);
			}
		}
		if (fileName.empty()) {
			printf("no filename specified\n");
			printUsageAndExit(exeName);
		}

		if (fileName.string().find('/') == std::string::npos) {
			fileName = (fs::current_path() / fileName);
		}

		if (!fs::exists(fileName)) {
			fs::ofstream outFile(fileName);
			auto newTemplate = appTemplate;
			replaceAll(newTemplate, "APP_NAME", fileName.stem().string());
			outFile << newTemplate;
			outFile.close();
		}

		setupAudio();

		dylib.willCloseDylib = [&]() {
			if (eventDispatcher) eventDispatcher->exit();
			eventDispatcher = nullptr;
		};

		dylib.recompiled = [&](std::shared_ptr<App> app) {
			eventDispatcher = std::make_shared<EventDispatcher>(app);
			eventDispatcher->setup();
			eventDispatcher->resized();
		};
		printf("Using MZGL root: %s\n", mzglRoot.c_str());
		dylib.setup(fileName, mzglRoot, includes);

		fs::path srcDir = fs::path(fileName).parent_path();
		for (const auto &entry: fs::directory_iterator(srcDir)) {
			auto p = entry.path();
			if (p.extension() == ".h" && p != fileName) {
				dylib.addFileToWatch(p.string());
				printf("watching %s\n", p.filename().c_str());
			}
		}

		if (shouldOpenInVsCode) {
			execute("code " + fileName.string());
		}
	}

	moodycamel::ReaderWriterQueue<MidiMessage> midiQueue {200};

	void midiReceived(const std::shared_ptr<MidiDevice> &device,
					  const MidiMessage &m,
					  uint64_t timestamp) override {
		if (!eventDispatcher) return;
		midiQueue.enqueue(m);
	}
	void setupAudio() {
		audioSystem = std::make_shared<AudioSystem>();
		audioSystem->setBufferSize(64);
		audioSystem->setup(2, 2);
		audioSystem->bindToApp(this);
		audioSystem->start();
	}
	void audioIn(float *data, int frames, int chans) override {
		if (dylib.tryLock()) {
			if (eventDispatcher) eventDispatcher->app->audioIn(data, frames, chans);

			dylib.unlock();
		}
	}
	void audioOut(float *data, int frames, int chans) override {
		if (dylib.tryLock()) {
			if (eventDispatcher) {
				MidiMessage msg;
				while (midiQueue.try_dequeue(msg)) {
					if (auto midiListener = dynamic_cast<MidiListener *>(eventDispatcher->app.get())) {
						midiListener->midiReceived(nullptr, msg, 0);
					}
				}
				eventDispatcher->app->audioOut(data, frames, chans);
			}
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
		if (key == 114) {
			dylib.recompile();
		}
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