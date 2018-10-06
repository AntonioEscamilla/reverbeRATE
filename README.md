# reverbeRATE
## Presentation
The software **reverbeRATE** is a desktop application that simulates the natural reverberation of an acoustic enclosure, on the user's own voice and in real time. The application uses the convoLib library (developed by the same authors in the framework of the research project 771B-06 / 17-23) to calculate the convolution operation in real time, between the response to the impulse of some acoustic system and a voice signal.

reverbeRATE is presented as a use and implementation case, with which the main performance characteristics of the convoLib library could be evidenced, as well as detecting faults and possible improvements on the convolution engine. The above means that the application is presented thanks to a mature and fully developed version of the convolution engine in real time. Additionally, the application aims to create awareness of the acoustic heritage that represents the response to the impulse of an enclosure and generate familiarity in the user on the use of the mathematical model and the procedure that allows to print on a voice signal, the acoustic characteristics of room.

## Requirements
**reverbeRATE** can currently only be used on computers with Mac OS X operating system, versions 10.10 or later. For its installation it is recommended to drag the reverbeRATE.app file to the / Applications folder. And be executed from this location.

For the use of the application it is necessary to have enabled an audio input from the computer microphone (or an audio interface) and a stereo output. It is advisable to use headphones to listen to the reverberated signal, so that there is no feedback effect when captured by the active microphone for recording. In case there is a rapid increase in the level of the output signal, disable the audio output of the system using the quick access from the keyboard.

## Running the App
**reverbeRATE** presents a simple and minimalist user interface, to facilitate the use of the App. The main window only has two buttons to control the application, as shown in Figure 1.

![reverberate_1](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/reverbeRATE/reverbeRATE_1.png)

### 1. Load an Impulse Response
The green button on the left shows a small representation of an impulse response and when you click on it, a menu with a list of impulse responses available for the convolution is displayed. The set of responses to impulse seeks to be broad and diverse, so it is possible to use responses to the impulse of venues such as a church, a government palace, an auditorium, a silo and mechanical systems such as a plate or a spring. See Figure 2.

![reverberate_2](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/reverbeRATE/reverbeRATE_2.png)

### 2. Real Time Convolution
The button on the right, initially inactive in black and white, represents the diffusivity of the acoustic field that is generated in the enclosures and the sensation of filling all the space that produces the reverberation. Clicking on this button activates the capture of the signal coming from the microphone, the convolution process in real time with the response to the selected impulse and the activation of the stereo output with the result. The toggle type button activates or deactivates this state and allows to interrupt the audio processing. See Figure 3.

![reverberate_3](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/reverbeRATE/reverbeRATE_3.png)

## About this Software
reverbeRATE was developed by **Antonio Escamilla Pinilla** and **José Ricardo Zapata González** working for the Universidad Pontificia Bolivariana, in the context of a research project entitled **Software Development for Measurement, Processing and Analysis of Acoustic Impulse Responses**. Project funded by the Research Center for Development and Innovation CIDI-UPB with number 771B-06/17-23.
