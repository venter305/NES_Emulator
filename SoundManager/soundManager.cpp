#include "soundManager.h"
#include <iostream>
#include <cmath>

pa_stream *SoundManager::stream = NULL;
int SoundManager::bufSize = 48000*2;
uint8_t *SoundManager::soundBuf = new uint8_t[SoundManager::bufSize];
int SoundManager::outBufIndex = 0;
int SoundManager::samplesNeeded = 0;
bool SoundManager::running = true;
uint16_t (*SoundManager::UserFunc)() = 0;
std::string SoundManager::name = "";

void SoundManager::stream_write_callback(pa_stream *s, size_t length, void *userdata){

	samplesNeeded = length;
	uint8_t buf[samplesNeeded];
	for (int i=0;i<samplesNeeded;i+=2){
		uint16_t sample = UserFunc();
		buf[i+1] = (sample&0xFF00)/0x100;
		buf[i] = sample&0x00FF;
	}
	pa_stream_write(s,buf,length,NULL,0,PA_SEEK_RELATIVE);

}

void SoundManager::stream_state_callback(pa_stream *s, void *userdata){
}

void SoundManager::context_state_callback(pa_context *c, void *userdata){
	if (pa_context_get_state(c) != PA_CONTEXT_READY)
		return;

	static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 48000,
        .channels = 1
    };

		int bufSize1 = 1024*8;

		const pa_buffer_attr bufAttr = {
		.maxlength = (uint32_t)bufSize1,
		.tlength = (uint32_t)-1,
		.prebuf = (uint32_t)-1,
		.minreq = (uint32_t)-1,
		.fragsize = (uint32_t)-1
	};



	stream = pa_stream_new(c,name.c_str(),&ss,NULL);
	pa_stream_set_state_callback(stream,stream_state_callback,NULL);
	pa_stream_set_write_callback(stream,stream_write_callback,NULL);
	pa_stream_connect_playback(stream,NULL,&bufAttr,(pa_stream_flags_t)0,NULL,NULL);
}

SoundManager::SoundManager(std::string _name){
	int ret = 0;

	name = _name;

	s = pa_threaded_mainloop_new();
	mainloop_api = pa_threaded_mainloop_get_api(s);
	context = pa_context_new(mainloop_api,name.c_str());

	pa_context_set_state_callback(context,context_state_callback,NULL);

	pa_context_connect(context,NULL,(pa_context_flags_t)0,NULL);

	pa_threaded_mainloop_start(s);
}

SoundManager::~SoundManager(){
	int ret = 0;
	running = false;
	mainloop_api->quit(mainloop_api,ret);
	pa_stream_disconnect(stream);
	pa_context_disconnect(context);
	pa_threaded_mainloop_stop(s);
	pa_threaded_mainloop_free(s);
}

int max1 = 124;
	int count = 48000;
	int mask = 1;
	int j = 1;
	int duty = 0b00001111;

void SoundManager::clock(){
	std::cout << "test" << std::endl;

	soundBuf[sBufIndex] = (output);
	if (sBufIndex == bufSize-1){
		sBufIndex = 0;
	}
	else
		sBufIndex++;
	samplesNeeded--;
}
