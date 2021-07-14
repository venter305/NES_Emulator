#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include <pulse/pulseaudio.h>
#include <fstream>

class SoundManager {
	public:
		pa_threaded_mainloop *s = NULL;
		pa_context *context = NULL;
		pa_mainloop_api *mainloop_api = NULL;
		static pa_stream *stream;

		static std::string name;

		static int bufSize;
		static uint8_t *soundBuf;
		static int outBufIndex;
		int sBufIndex = 0;

		uint8_t output = 0;

		static int samplesNeeded;
		static bool running;

		SoundManager(std::string _name);
		~SoundManager();

		static uint16_t (*UserFunc)();

		void clock();

		static void stream_state_callback(pa_stream *s, void *userdata);
		static void context_state_callback(pa_context *c, void *userdata);
		static void stream_write_callback(pa_stream *s, size_t length, void *userdata);
};

#endif
