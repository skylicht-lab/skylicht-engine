#import "AVFoundation/AVFoundation.h"

float audioGetStreamBufferLength()
{
    AVAudioSession *session = [AVAudioSession sharedInstance];
    return (float)session.IOBufferDuration;
}

void audioSetupStreamBufferLength(float time)
{
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    NSError* error = nil;
    [session setPreferredIOBufferDuration:time error:&error];
    if (error != nil)
    {
        printf("Error: audioSetupStreamBufferLength\n");
    }
}