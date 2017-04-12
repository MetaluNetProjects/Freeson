# Freeson

## Attempt of a musical instrument for deaf people.

This is the software of a machine built for giving to deaf people more reasons to come to a musical event.

This project was initiated by l'Aéronef(Lille) in 2016; the concept has been elaborated after several meetings and discussions with representants of the deaf community.

Research, design and making by metalu.net.

## Description

The installation is equipped with an input XLR connector, which can receive audio signal from a microphone, a media player, a mixing desk or a RF receiver.

A beat-tracking analys estimates the BPM and the beat phase of the incoming signal, and runs a slave clock based on this data.

4 classical phones are hacked to become deaf-friendly devices : in three of us we replaced the microphone and the speaker with cellphone vibrators, and the fourth one is equiped with electro-stimulation electrodes.

The classical phone keyboards allow to choose the rythmic pattern for each telephone, the vibrator ones being two-channel patterns (mouth side/ear side); two special keys on each phone allow to double/halt the tempo for this phone.

An hemispherical screen displays an history of the spectral analyis of the input signal, and the way it is shown is controlled by a special knob.

A bulb-lamp lights on the beat, and a "Manual" switch allows the slave clock to run independantly of the incoming audio signal, given the value of the BPM knob. 

## System

A read-only-system raspbian on a raspberryPi3 runs the PureData patch at startup.

The screen is displayed with Pof, with which GLSL shaders were experimented for the first time; shaders are used to control the way the spectrum history is shown, relating to the displayed period length and bandwidth, and to the compression factors in both axes.

Fraise drives all the electronics; 3 Versa are used for the 3 vibrator phones, and a 8X2A for the electro-stimulation phone, this particular device needing a 12V power bridge.  
The other side of the twin-bridge L298 chip present on the 8X2A is used to power the 12V bulb-lamp.


