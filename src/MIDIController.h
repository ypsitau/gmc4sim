// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __MIDICONTROLLER_H__
#define __MIDICONTROLLER_H__

#include <windows.h>
#include <mmsystem.h>

class MIDIController {
private:
	HMIDIOUT _hMIDI;
public:
	inline MIDIController() : _hMIDI(NULL) {}
	inline ~MIDIController() {
		Reset();
		Close();
	}
	static inline int GetNumDevs() {
		return ::midiOutGetNumDevs();
	}
	inline void Open() {
		if (_hMIDI != NULL) Close();
		::midiOutOpen(&_hMIDI, 0, 0, 0, CALLBACK_NULL);
	}
	inline void Close() {
		if (_hMIDI != NULL) ::midiOutClose(_hMIDI);
		_hMIDI = NULL;
	}
	inline void Reset() {
		if (_hMIDI != NULL) ::midiOutReset(_hMIDI);
	}
	inline void NoteOn(unsigned char channel, unsigned char note, unsigned char velocity) {
		ShortMsg(0x90 + channel, note, velocity);
	}
	inline void ProgramChange(unsigned char channel, unsigned char program) {
		ShortMsg(0xc0 + channel, program, 0);
	}
	inline void ShortMsg(unsigned char status, unsigned char data1, unsigned char data2) {
		if (_hMIDI != NULL) {
			::midiOutShortMsg(_hMIDI, ((DWORD)data2 << 16) | ((DWORD)data1 << 8) | status);
		}
	}
};

#endif
