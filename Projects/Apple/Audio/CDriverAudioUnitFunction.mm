#include <stdio.h>

#if defined(IOS)
#import <AVFoundation/AVFoundation.h>

float AudioGetStreamBufferLength()
{
    AVAudioSession *session = [AVAudioSession sharedInstance];
    return (float)session.IOBufferDuration;
}

void AudioSetupStreamBufferLength(float time)
{
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    NSError* error = nil;
    [session setPreferredIOBufferDuration:time error:&error];
    if (error != nil)
    {
        printf("[SkylichtAudio] Error: AudioSetupStreamBufferLength\n");
    }
}
#endif
