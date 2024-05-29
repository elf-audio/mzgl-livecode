
#include "App.h"

// Function to interpolate between two values
float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

// Function to fade the input value
float fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

// Function to compute the gradient
float grad(int hash, float x) {
	int h	   = hash & 15;
	float grad = 1 + (h & 7); // Gradient value is one of 1, 2, ..., 8
	if (h & 8) grad = -grad; // and a random sign
	return (grad * x); // Multiply the gradient with the distance
}

// Perlin noise function
float perlin1D(float x) {
	static const int perm[] = {
		151, 160, 137, 91,	90,	 15,  131, 13,	201, 95,  96,  53,	194, 233, 7,   225, 140, 36,  103, 30,	69,
		142, 8,	  99,  37,	240, 21,  10,  23,	190, 6,	  148, 247, 120, 234, 75,  0,	26,	 197, 62,  94,	252,
		219, 203, 117, 35,	11,	 32,  57,  177, 33,	 88,  237, 149, 56,	 87,  174, 20,	125, 136, 171, 168, 68,
		175, 74,  165, 71,	134, 139, 48,  27,	166, 77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230,
		220, 105, 92,  41,	55,	 46,  245, 40,	244, 102, 143, 54,	65,	 25,  63,  161, 1,	 216, 80,  73,	209,
		76,	 132, 187, 208, 89,	 18,  169, 200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186,
		3,	 64,  52,  217, 226, 250, 124, 123, 5,	 202, 38,  147, 118, 126, 255, 82,	85,	 212, 207, 206, 59,
		227, 47,  16,  58,	17,	 182, 189, 28,	42,	 223, 183, 170, 213, 119, 248, 152, 2,	 44,  154, 163, 70,
		221, 153, 101, 155, 167, 43,  172, 9,	129, 22,  39,  253, 19,	 98,  108, 110, 79,	 113, 224, 232, 178,
		185, 112, 104, 218, 246, 97,  228, 251, 34,	 242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,	51,
		145, 235, 249, 14,	239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184, 84,	204, 176, 115, 121, 50,
		45,	 127, 4,   150, 254, 138, 236, 205, 93,	 222, 114, 67,	29,	 24,  72,  243, 141, 128, 195, 78,	66,
		215, 61,  156, 180, 151, 160, 137, 91,	90,	 15,  131, 13,	201, 95,  96,  53,	194, 233, 7,   225, 140,
		36,	 103, 30,  69,	142, 8,	  99,  37,	240, 21,  10,  23,	190, 6,	  148, 247, 120, 234, 75,  0,	26,
		197, 62,  94,  252, 219, 203, 117, 35,	11,	 32,  57,  177, 33,	 88,  237, 149, 56,	 87,  174, 20,	125,
		136, 171, 168, 68,	175, 74,  165, 71,	134, 139, 48,  27,	166, 77,  146, 158, 231, 83,  111, 229, 122,
		60,	 211, 133, 230, 220, 105, 92,  41,	55,	 46,  245, 40,	244, 102, 143, 54,	65,	 25,  63,  161, 1,
		216, 80,  73,  209, 76,	 132, 187, 208, 89,	 18,  169, 200, 196, 135, 130, 116, 188, 159, 86,  164, 100,
		109, 198, 173, 186, 3,	 64,  52,  217, 226, 250, 124, 123, 5,	 202, 38,  147, 118, 126, 255, 82,	85,
		212, 207, 206, 59,	227, 47,  16,  58,	17,	 182, 189, 28,	42,	 223, 183, 170, 213, 119, 248, 152, 2,
		44,	 154, 163, 70,	221, 153, 101, 155, 167, 43,  172, 9,	129, 22,  39,  253, 19,	 98,  108, 110, 79,
		113, 224, 232, 178, 185, 112, 104, 218, 246, 97,  228, 251, 34,	 242, 193, 238, 210, 144, 12,  191, 179,
		162, 241, 81,  51,	145, 235, 249, 14,	239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184, 84,	204,
		176, 115, 121, 50,	45,	 127, 4,   150, 254, 138, 236, 205, 93,	 222, 114, 67,	29,	 24,  72,  243, 141,
		128, 195, 78,  66,	215, 61,  156, 180};

	int X = static_cast<int>(floor(x)) & 255; // Find unit grid cell containing x
	x -= floor(x); // Get relative x coordinate in that cell
	float u = fade(x); // Compute fade curves for x

	int a = perm[X];
	int b = perm[X + 1];

	// And add blended results from 2 corners of the square
	return lerp(grad(a, x), grad(b, x - 1), u);
}
#include "DropDown.h"
#include "Slider.h"
float pitch	   = 440;
float lfoFreq  = 2;
float lfoDepth = 0.7;
enum class LfoType { Sine, Square, Beep };

LfoType lfoType = LfoType::Beep;
class MyApp : public App {
public:
	MyApp(Graphics &g)
		: App(g) {
		auto *dropDown	   = new DropDown(g, {"sine", "square", "beep"});
		dropDown->onChange = [&](int i) { lfoType = static_cast<LfoType>(i); };
		root->addChildren({dropDown,
						   new Slider(g, "freq", pitch, 30, 9000, 4),
						   new Slider(g, "lfo freq", lfoFreq, 0.1, 20, 4),
						   new Slider(g, "lfo depth", lfoDepth)});
	}

	void resized() override {
		int sp = 10;
		root->position(sp, sp);
		root->size(200, root->getNumChildren() * 50);
		root->layoutChildrenAsGrid(1, root->getNumChildren(), sp);
	}
	float xx = 0;
	float yy = 0;

	float smoothedLfo = 0;
	void draw() override {
		float m		= 0.25;
		smoothedLfo = currVal * m + smoothedLfo * (1 - m);
		mapf(currVal, -1, 1, 0.2, 0.8);
		vec4 green = {0, 1, 0, 1};
		vec4 red   = {1, 0, 0, 1};

		vec4 col = glm::mix(green, red, smoothedLfo);
		col		 = glm::mix(col, vec4(0.5, 0.5, 0.5, 1), 0.75);
		g.clear(col);

		ScopedAlphaBlend bl(g, true);

		std::vector<vec2> vs;
		g.setColor(1);
		for (int i = 0; i < g.width; i++) {
			vs.push_back({i, mapf(lfo.osc(i / (float) g.width), -1.1, 1.1, g.height, 0)});
		}
		g.drawLineStrip(vs);
	}
	void touchOver(float x, float y) override {
		xx = x;
		yy = y;
	}
	double dd = 0;

	// Sigmoid function for smooth transitions
	float sigmoid(float x) {
		return 1.0f / (1.0f + std::exp(-x * 10.0f)); // Adjust the factor to control the slope
	}

	// Function to generate a more slopey analog-style square wave
	float analogSquareWave(double normPhase) {
		float sineValue	   = std::sin(normPhase * 2 * M_PI);
		float sigmoidValue = sigmoid(sineValue);
		return sigmoidValue * 2 - 1; // Scale to -1 to 1 range
	}

	class LFO {
	public:
		LfoType type = LfoType::Sine;
		double phase = 0;
		float freq	 = 4;
		float getSample() {
			phase += 2.f * M_PI * freq / 48000.f;

			float out = osc(static_cast<float>(phase / (2 * M_PI)));
			return out;
		}

		// Smoothstep function for smooth transitions
		float smoothstep(float edge0, float edge1, float x) {
			x = (x - edge0) / (edge1 - edge0); // Scale, bias and saturate x to 0..1 range
			x = std::clamp(x, 0.0f, 1.0f);
			return x * x * (3 - 2 * x);
		}

		// Function to generate a sawtooth wave
		float sawtoothWave(double phase) {
			double t = std::fmod(phase, 1);
			return 2.0 * (t) -1.0; // Scale to -1 to 1 range
		}

		float fastApproximateSqrt(float x) {
			if (x <= 0.0f) return 0.0f;
			if (x >= 1.0f) return 1.0f;

			// Initial guess
			float xhalf = 0.5f * x;
			int32_t i	= *reinterpret_cast<int32_t *>(&x); // Convert to int32 for bitwise manipulation
			i			= 0x1FBD1DF5 + (i >> 1); // Initial guess based on magic number
			float y		= *reinterpret_cast<float *>(&i); // Convert back to float

			// One iteration of Newton's method to improve the approximation
			y = y * (1.5f - xhalf * y * y);

			return y;
		}
		float bend = 0.7;
		float sm(float s) { return sin(s * M_PI * bend); }
		float decay = 0.1;
		float sq(double np) {
			float out = 1;
			if (np > 0.5) {
				out = -1;
				out += sm(np - 0.5) * decay;
			} else {
				out -= sm(np) * decay;
			}

			return out;
		}
		float osc(double normPhase) {
			if (type == LfoType::Sine) return sin(normPhase * 2 * M_PI);

			return sq(std::fmod(normPhase, 1)); //
				//	analogSquareWave(normPhase); // *
				//sawtoothWave(normPhase * 2);
			//			return sin(normPhase * 2 * M_PI);
		}
	};
	LFO lfo;
	std::atomic<float> currVal {0.f};
	float amp = 1;
	void audioOut(float *outs, int frames, int chans) override {
		lfo.freq	 = lfoFreq;
		lfo.type	 = lfoType;
		float lfoVal = 0;
		for (int i = 0; i < frames; i++) {
			lfoVal	   = lfo.getSample();
			float freq = pitch + pitch * lfoDepth * pow(2, lfoVal);
			freq += perlin1D(dd * 0.03) * 0.5;
			float x = analogSquareWave(dd);
			dd += freq / 48000.f;

			if (lfo.type == LfoType::Beep) {
				if (std::fmod(dd, 1) > 0.5) {
					amp = 0;
				} else {
					amp = 1;
				}

			} else {
				amp = 1;
			}
			amp = std::clamp(amp, 0.f, 1.f);
			x *= amp * 0.3;
			outs[i * chans]		= x;
			outs[i * chans + 1] = x;
		}
		currVal = lfoVal;
	}
};
