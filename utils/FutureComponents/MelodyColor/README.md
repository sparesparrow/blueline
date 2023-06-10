# MelodyColor

MelodyColor is an audio visualization application built with Qt6 and QML. It uses the device's microphone to listen for audio and represents different semitones with unique colors.

The project is organized in a way that the business logic, including audio processing and frequency to semitone mapping, is handled by C++ code using Qt6, while the user interface and graphics are managed by QML. 
This allows for a clear separation of concerns, which makes the application easier to maintain and extend.
The audio processing will be done in a SpectrumAnalyser class in C++. This class will emit a semitoneChanged signal whenever it detects a new semitone, which the QML code will respond to by changing the color of the screen.


### Project structure:

- main.cpp: Sets up the QML application and exposes the SpectrumAnalyser class to QML.
- SpectrumAnalyser.h: Implements the SpectrumAnalyser class, which performs audio processing and emits a signal when the semitone changes.
- SpectrumAnalyser.cpp: Implements the SpectrumAnalyser class, which performs audio processing and emits a signal when the semitone changes.
- main.qml: Defines the user interface and listens for the semitoneChanged signal to change the screen color.
- qml.qrc: A resource file that contains the QML files used in the project.

## Qt Components considered for this project:

- **Qt Multimedia Module Classes:**
  - `QAudioRecorder`: You can use this class to record audio from the microphone.
  - `QAudioProbe`: This class allows you to monitor audio being played or recorded, which you can use to analyze the audio data in real-time.
  - `QAudioBuffer`: Represents a collection of audio samples with a specific format and sample rate. You might need to use this class when processing the audio data.
  - `QAudioFormat`: Stores audio stream parameter information. You might need to use this class when setting up the QAudioRecorder.

- **Qt Multimedia QML Types:**
  - `Audio`: If you want to give feedback to the user, you can use this type to play a sound.
  - `VideoOutput`: If you plan to show a video visualization of the sound, you can use this type.

- **Qt Audio Engine:**
  - `AudioEngine`: If you want to do more complex audio processing, like adding 3d audio effects, you might need to use this class.
  - `Sound`: You could potentially use this to define different sounds that correspond to different semitones.



