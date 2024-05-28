
#include "LiveCodeApp.h"

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

class MyApp : public LiveCodeApp {
public:
	MyApp(Graphics &g)
		: LiveCodeApp(g) {}

	float xx = 0;
	float yy = 0;
	void draw() override {
		g.clear(0);
		ScopedAlphaBlend bl(g, true);
		g.setColor(0, 0, 0, 0.015);
		g.drawRect(0, 0, g.width, g.height);
		g.setColor(1, yy / g.height, randuf());
		//		g.setColor(randuf(), randuf() * 0.5, randuf());
		float radius = sin(g.currFrameTime * 4) * 100;
		float x		 = mapf(perlin1D(g.currFrameTime * xx / 100.f), -3, 3, 0, g.width);
		float y		 = mapf(perlin1D(g.currFrameTime + 200), -3, 3, 0, g.height);
		g.drawCircle(x, y, radius);
	}
	void touchOver(float x, float y) override {
		xx = x;
		yy = y;
	}
	double dd = 0;
	void audioOut(float *outs, int frames, int chans) override {
		for (int i = 0; i < frames; i++) {
			float x	  = sin(dd); //perlin1D(g.currFrameTime * 0.1f);
			float oct = mapf(yy, 0, g.height, 2, 0.5);
			dd += 256.f * oct * M_PI / 48000.f;
			outs[i * chans]		= x;
			outs[i * chans + 1] = x;
		}
	}
};
