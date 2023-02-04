//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2023 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <atomic>

#include <Windows.h>
#include <Mmsystem.h>
#include <Mmreg.h>
#include <KS.h>
#include <Ksmedia.h>
#include <dsound.h>

#include <musikcore/sdk/IOutput.h>
#include <musikcore/sdk/IDevice.h>

using namespace musik::core::sdk;

class DirectSoundOut : public IOutput {
    public:
        DirectSoundOut();
        ~DirectSoundOut();

        /* IPlugin */
        const char* Name() override { return "DirectSound"; };
        void Release() override;

        /* IOutput */
        void Pause() override;
        void Resume() override;
        void SetVolume(double volume) override;
        double GetVolume() override;
        void Stop() override;
        OutputState Play(IBuffer *buffer, IBufferProvider *provider) override;
        double Latency() override;
        void Drain() override;
        IDeviceList* GetDeviceList() override;
        bool SetDefaultDevice(const char* deviceId) override;
        IDevice* GetDefaultDevice() override;
        int GetDefaultSampleRate() override { return -1; }

    private:
        enum State {
            StateStopped,
            StatePlaying,
            StatePaused
        };

        bool Configure(IBuffer *buffer);
        void Reset();
        void ResetBuffers();
        LPCGUID GetPreferredDeviceId();

        std::atomic<State> state;

        WAVEFORMATEXTENSIBLE waveFormat;
        IDirectSound8 *outputContext;
        IDirectSoundBuffer *primaryBuffer;
        IDirectSoundBuffer8 *secondaryBuffer;
        DWORD bufferSize;
        DWORD writeOffset;
        int rate;
        int channels;
        double volume;
        double latency;
        bool firstBufferWritten;
        std::recursive_mutex stateMutex;
};
