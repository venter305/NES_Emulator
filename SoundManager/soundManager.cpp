#include "soundManager.h"
#include <iostream>
#include <cmath>

using namespace std;

pa_stream *SoundManager::stream = NULL;
int SoundManager::bufSize = 48000*2;
uint8_t *SoundManager::soundBuf = new uint8_t[SoundManager::bufSize];
int SoundManager::outBufIndex = 0;
int SoundManager::samplesNeeded = 0;
bool SoundManager::running = true;

void SoundManager::stream_write_callback(pa_stream *s, size_t length, void *userdata){

	samplesNeeded = length;
	uint8_t buf[length];

	//cout << "--------" << length << endl;
	while(samplesNeeded > 0 && running){}
	for (int i=0;i<length;i++){
		buf[i] = soundBuf[outBufIndex];
		//cout << (int)buf[i] << ' ' << soundBuf[outBufIndex] << endl;
		if (outBufIndex == (bufSize-1))
			outBufIndex = 0;
		else
			outBufIndex++;
	}
	//cout << outBufIndex << endl;
	pa_stream_write(s,buf,length,NULL,0,PA_SEEK_RELATIVE);
	
}

void SoundManager::stream_state_callback(pa_stream *s, void *userdata){
	cout << "Stream Test" << endl;
}	

void SoundManager::context_state_callback(pa_context *c, void *userdata){
	cout << "Context Test" << endl;
	if (pa_context_get_state(c) != PA_CONTEXT_READY)
		return;

	static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 48000,
        .channels = 2
    };

		int bufSize1 = 48000*2;

		const pa_buffer_attr bufAttr = {
		.maxlength = (uint32_t)bufSize1,
		.tlength = (uint32_t)bufSize1,
		.prebuf = (uint32_t)-1,
		.minreq = (uint32_t)bufSize1,
		.fragsize = (uint32_t)-1		
	};
	
	
	
	stream = pa_stream_new(c,"Test",&ss,NULL);
pa_stream_set_state_callback(stream,stream_state_callback,NULL);		  pa_stream_set_write_callback(stream,stream_write_callback,NULL);
	pa_stream_connect_playback(stream,NULL,NULL,(pa_stream_flags_t)0,NULL,NULL);
}

SoundManager::SoundManager(){
	int ret = 0;

	s = pa_threaded_mainloop_new();
	mainloop_api = pa_threaded_mainloop_get_api(s);
	context = pa_context_new(mainloop_api,"Test");
	
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
			
	soundBuf[sBufIndex] = (output);
	if (sBufIndex == bufSize-1){
		sBufIndex = 0;
	}
	else
		sBufIndex++;

	samplesNeeded--;
}
