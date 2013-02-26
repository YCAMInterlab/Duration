# [Duration](http://duration.cc)

## Timeline for creative code 

Duration controls change over time. With a simple one window approach, the cross platform stand alone application manages lists of tracks to compose changing data over a fixed duration.

The application sends values over OSC and can be configured through OSC messages.

![DurationInterface](http://www.duration.cc/assets/images/duration_screen.png)

Duration is open source and completely free to use in your art, research, or commercial projects. It's built with [openFrameworks](http://www.openframeworks.cc), and inherits its commitment to sharing. Duration is a project by [James George](http://www.jamesgeorge.org), co-developed by [YCAMInterLab](http://interlab.ycam.jp/en/) during the [Guest Research Project v.2](http://interlab.ycam.jp/en/projects/guestresearch/vol2)

## Download

Download Duration: 

[Mac OS X 10.6+](http://www.duration.cc/downloads/Duration_003_OSX.zip)

[Windows XP and 7](http://www.duration.cc/downloads/Duration_003_Windows.zip)

[Linux 64bit](http://www.duration.cc/downloads/Duration_003_Linux64.zip) 

Thanks to [Philip Whitfield](http://underdoeg.com/) for the Linux build.

[Source for all platforms](http://www.duration.cc/downloads/DurationSource_003.zip)

For Windows, you may need to install the [MSVC++ Runtime](http://www.microsoft.com/en-us/download/details.aspx?id=5555) and [OpenAL](http://connect.creativelabs.com/developer/Wiki/OpenAL%20Installer%20for%20Windows.aspx)

## Demo video

Readme TL;DR? The latest walk through video is here:
https://vimeo.com/59654979


## License

    Copyright 2012-2013 James George and YCAMInterLab

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


## Changing the default language

Duration is currently available in English, Japanese, French and Italian.

To change the language, open Duration/data/settings.mxl and change the word inside the language tag. Restart Duration

Thanks to Joanie LeMercier, Takayuki Ito, and Francesco Fantoni for help translating! If you would like to provide a translation to a different language, please take a look at the languagefile.csv, add your translations and send it to me through a pull request or email and I'll add it to the next release!

## Using the Duration interface

### Managing Projects

A Duration project is just a collection of track data and interface configuration settings. It's all saved to a folder with .xml files and a special .durationproj file containing the track names and settings.

By convention Duration looks for projects in ~/Documents/Duration folder. Projects saved here will show up in the Project drop-down menu within the application. You can put projects anywhere else you'd like, but you will have to remember yourself where you put them.

### Managing tracks

Tracks are everything in Duration. To add tracks to your project select a type from the Add Track drop-down and it'll be appended to your composition. Name the track by clicking on the the name in the track header and typing in a new name. This name is used for all outgoing OSC messages to identify your track, so think of it as both a name and an OSC address. You can have duplicate names, but it makes it hard to work with on the output side. It's also common to put routing information right in the name, so /mytrack/position/x is a great name for a track. If you leave off the leading slash Duration adds it for you when it sends the message.

To remove a track, click on the delete button on the far right of the header and confirm you want it removed.

### Track Types

Duration has six built in track types. Each track has keyframes which specify values or behavior at specific moments. Between keyframes tracks can define ways of interpolating values to create smooth changes.

#### Bangs

Bangs are the simplest track, causing a message with no parameters to be triggered once when the playhead passes over each keyframe.

#### Flags

Flags are just like bangs with the added functionality of attaching a string of text to be attached to the message.

#### Switches

Switches cause large regions of the track to be marked as on, and messages are sent each time a switch edge is encountered. 

#### Curves

Curves allow for specifying a smoothly changing value between a given min and max range. Right clicking a keyframe brings up a menu for selecting different smooth interpolation options between keyframes.

#### LFOs

Low Frequency Oscillators provide sine waves and noise functions to be used in animations.

#### Colors

Color tracks use a color palette image loaded to create smoothly changing colors over time. Each sample on the timeline specifies a position in the palette image to sample from. The next position is smoothly transitioned, sampling from the image along the way. 

#### Audio

Audio track allows for visualizing an audio waveform, sound playback, and FFT data sent through OSC. Currently only one audio track is allowed per project, and the duration of the project is fixed to the length of the audio track.

### Setting the duration

Every project has a fixed Duration. To change it, click on the duration timecode value beneath the playhead current time code on the to panel and type a new duration. Your new duration value must match the timecode format of HH:MM:SS:MILS. Shortening the duration may result in some keyframes being clipped if they fall out of range.

### In and Out points

Setting in and out points let's you focus playback on just a small part of your composition. Use the hotkeys i and o to set the in and out range, or drag the handle beneath the time ticker. Alt+i and Alt+o can be used to clear the current in out.

### Configuring BPM

Duration has a very basic 4/4 time signature grid based on a BPM. This let's you snap keyframes to precise quarter measures. To change the BPM click and drag the BPM digit on the top panel.

Note: more complex time signatures are planned for future releases.

### Configuring OSC communication

The top right of the Duration window configures incoming and outgoing OSC for the project. Type the incoming port to listen to and the outgoing address and port.  Each individual track may also be enabled or disabled using the toggle in the header.

### Shortcuts

* NOTE: Windows and Linux use CTRL and Mac uses COMMAND 

<table>
    <tr>
        <td style="font-weight:bold">Function</td><td style="font-weight:bold">Shortcut</td>
    </tr>
    <tr>
        <td>Toggle play</td><td>SPACE</td>
    </tr>
    <tr>
        <td>Save</td><td>command+s</td>
    </tr>
    <tr>
        <td>Cut Selection</td><td>command+x</td>
    </tr>
    <tr>
        <td>Copy Selection</td><td>command+c</td>
    </tr>
    <tr>
        <td>Paste Selection</td><td>command+v</td>
    </tr>
    <tr>
        <td>Undo</td><td>command+z</td>
    </tr>
    <tr>
        <td>Redo</td><td>shift+command+z</td>
    </tr>
    <tr>
        <td>Set in point</td><td>i</td>
    </tr>
    <tr>
        <td>Set out point</td><td>o</td>
    </tr>
    <tr>
        <td>Clear in point</td><td>alt+i</td>
    </tr>
    <tr>
        <td>Clear out point</td><td>alt+o</td>
    </tr>
    <tr>
        <td>Select all keyframes in Focused track</td><td>command+a</td>
    </tr>
    <tr>
        <td>Add all keyframes in Focused track to selection</td><td>command+shift+a</td>
    </tr>
    <tr>
        <td>Delete all selected keyframes</td><td>delete or backspace</td>
    </tr>    
    <tr>
        <td>Nudge keyframes a little</td><td>arrow keys</td>
    </tr>    
    <tr>
        <td>Nudge keyframes a little more</td><td>shift+arrow keys</td>
    </tr>    

</table>

## Connecting things to Duration

By itself Duration is pretty useless, so let's hook it up to another realtime environment through OSC.

### Receiving output

Duration sends all its data over OSC in bundles. Whenever a value changes it well send an update (whether you are just editing the track or it's playing back). The OSC messages are formatted like this:

     /track/display/name <values as arguments>

Each type of track sends different arguments.
<table>
    <tr>
        <td style="font-weight: bold">Track Type</td>
        <td style="font-weight: bold">Arguments</td>
    </tr>
    <tr>
        <td>Bang</td>
        <td>No Arguments.</td>
    </tr>
    <tr>
        <td>Flag</td>
        <td>1 String argument representing Flag's text (can be blank)</td>
        </tr>
    <tr>
        <td>Switch</td>
        <td>1 Int32 argument, set to 1 for on, 0 for off</td>
    </tr>
    <tr>
        <td>Curve and LFO</td>
        <td>1 Float argument representing the current value, ranging between Min and Max</td>
    </tr>
    <tr>
        <td>Color</td>
        <td>3 Int32 arguments ranging from 0-255 for Red, Green, Blue</td>
    </tr>
</table>


### Track info messages

When a new project is loaded or playback begins, Duration will always send a special information message with the address /duration/info. The arguments contain information about all the tracks in the current project. The message has 4 arguments per Curves track, and 2 arguments for other tracks. The first argument is always a string with the track type, the second is the display name (which will always match the address of messages sent from that track). For Curves, the additional two arguments are floats representing the min and max values for that track.

For a project with 2 color tracks, a curves track and a flags track the info message would look like this:
     
    /duration/info Colors /colortrack_one Colors /colortrack_two Curves /accelerate -1.5 2.0 Flags /flagtrack

### Controlling Duration through OSC

Duration's OSC input is always listening for you to tell it what to do next.

<table>
    <tr>
        <td style="font-weight: bold">Command</td>
        <td style="font-weight: bold">OSC Address</td>
        <td style="font-weight: bold">Arguments</td>
        <td style="font-weight: bold">Notes</td>
    </tr>
    <tr>
        <td>New Project</td>
        <td>/duration/new</td>
        <td>ProjectPath (String)</td>
        <td>Immediately creates and opens a new project for "ProjectPath", Current project is not saved. Paths with a leading slash are assumed absolute, otherwise relative to ~/Documents/Duration</td>
    </tr>
    
    <tr>
        <td>Open project</td>
        <td>/duration/open</td>
        <td>ProjectPath (String)</td>
        <td>Immediately opens project at "ProjectPath", Current project is not saved. Paths with a leading slash are assumed absolute, otherwise relative to ~/Documents/Duration</td>
    </tr>
    <tr>
        <td>Save Project</td>
        <td>/duration/save</td>
        <td>-</td>
        <td>Saves any changes to the current project.</td>
    </tr>

    <tr>
        <td>Start Playback</td>
        <td>/duration/play</td>
        <td>-</td>
        <td>Does not have an effect if the project is already playing. If playback started an /duration/info message is triggered</td>
    </tr>
    <tr>
        <td>Stop Playback</td>
        <td>/duration/stop</td>
        <td>-</td>
        <td>If already stopped, this command will reset the playhead to time 0</td>
    </tr>
    <tr>
        <td>Set Duration</td>
        <td>/duration/setduration</td>
        <td>Seconds (Float)</td>
        <td>Resets the current project's Duration to Seconds. If the duration is shortened values may be clipped</td>
    </tr>
    <tr>
        <td>Set Duration</td>
        <td>/duration/setduration</td>
        <td>Milliseconds (Int)</td>
        <td>Resets the current project's Duration to Millis. If the duration is shortened values may be clipped</td>
    </tr>
    <tr>
        <td>Set Duration</td>
        <td>/duration/setduration</td>
        <td>Timecode (String)</td>
        <td>Resets the current project's Duration to Timecode, formatted HH:MM:SS:MILS. If the duration is shortened values may be clipped</td>
    </tr>
    <tr>
        <td>Seek</td>
        <td>/duration/seektoposition</td>
        <td>Position (Float)</td>
        <td>Sets playhead position to Position, 0 to 1.0</td>
    </tr>
    <tr>
        <td>Seek</td>
        <td>/duration/seektosecond</td>
        <td>Second (Float)</td>
        <td>Sets playhead position to the specified second</td>
    </tr>
    <tr>
        <td>Seek</td>
        <td>/duration/seektomillis</td>
        <td>Millis (Int)</td>
        <td>Sets playhead position to the specified millisecond</td>
    </tr>
    <tr>
        <td>Seek</td>
        <td>/duration/seektotimecode</td>
        <td>Millis (String)</td>
        <td>Sets playhead position to the specified timecode, specified HH:MM:SS:MILS</td>
    </tr>
    <tr>
        <td>Enable OSC In</td>
        <td>/duration/enableoscin</td>
        <td>Enable (Int)</td>
        <td>1 enables, 0 disables incoming OSC. Careful this may be the last message you ever send.</td>
    </tr>
    <tr>
        <td>Enable OSC In</td>
        <td>/duration/enableoscin</td>
        <td>TrackName (String); Enable (Int)</td>
        <td>1 enables, 0 disables incoming OSC for TrackName</td>
    </tr>
    <tr>
        <td>Enable OSC Out</td>
        <td>/duration/enableoscout</td>
        <td>Enable (Int)</td>
        <td>1 enables, 0 disables outgoing OSC</td>
    </tr>
    <tr>
        <td>Enable OSC Out</td>
        <td>/duration/enableoscout</td>
        <td>TrackName (String); Enable (Int)</td>
        <td>1 enables, 0 disables outgoing OSC on track matching name</td>
    </tr>
    <tr>
        <td>Enable OSC Out</td>
        <td>/duration/enableoscout</td>
        <td>TrackName (String); Enable (Int)</td>
        <td>1 enables, 0 disables outgoing OSC on track matching name</td>
    </tr>
    <tr>
        <td>OSC Rate</td>
        <td>/duration/oscrate</td>
        <td>Rate (Float)</td>
        <td>Sets the outgoing OSC rate in messages per second. Default is 30</td>
    </tr>
    <tr>
        <td>Add Track</td>
        <td>/duration/addtrack</td>
        <td>TrackType (String); TrackName (String,Optional); XMLFile (String,Optional)</td>
        <td>Creates a new track of type TrackType (bangs, flags, switches, curves, colors or audio. Optionally set the name and a path to an external XML file to load.</td>
    </tr>
    <tr>
        <td>Set Range</td>
        <td>/duration/valuerange</td>
        <td>TrackName (String); Min (Float); Max (Float)</td>
        <td>Changes the value range of the target Curves track to [Min, Max]</td>
    </tr>
    <tr>
        <td>Set Range: Min</td>
        <td>/duration/valuerange/min</td>
        <td>TrackName (String); Min (Float)</td>
        <td>Changes the min value of the target Curves track to Min</td>
    </tr>
    <tr>
        <td>Set Range: Max</td>
        <td>/duration/valuerange/max</td>
        <td>TrackName (String); Max (Float)</td>
        <td>Changes the min value of the target Curves track to Max</td>
    </tr>
    <tr>
        <td>Color Palette</td>
        <td>/duration/colorpalette</td>
        <td>TrackName (String); PalettePath (String)</td>
        <td>Loads the image at PalettePath for target Colors track</td>
    </tr>
    <tr>
        <td>Audio Clip</td>
        <td>/duration/audioclip</td>
        <td>ClipPath (String)</td>
        <td>Loads the sound clip into the Audio track. The Audio track must alrady be present in the current project</td>
    </tr>

</table>

### Remote Control 

Duration comes packaged with a simple application called "DurationRemote", which can be used to test control messages.

![DurationRemote](http://www.duration.cc/duration_remote.png)

### Recording data into tracks from OSC

Duration has an experimental feature for recording OSC data as keyframes in Curves or Bang tracks. To record an incoming signal into a curves track, rename a track to match the address of the message being received. The first parameter of the incoming message must also be a float. 

If a message is being received that matches a Curve track's name and has valid data, the background of that track will start to pulse red, indicating it's ready to record. Hitting play will immediately result in the incoming data to be written into the track. Use a Bang track to record intermittent messages and a Curves track to record data streams. To simply play a track back without recording any data, disable incoming OSC on that track.

* NOTE: Use the bundled RecordingDataGenerator to test this functionality by naming a track to match one of the outgoing parameters.

![RecordingGenerator](http://www.duration.cc/duration_datagenerator.png)

* NOTE: This is still an experimental feature with a few quirks. Be careful about receiving values out of range, recording over existing data or ending up with huge amounts of unmanageable keyframes! All of these cases are not yet handled very well by Duration.

## Hacking on Duration

Duration is open source and free to use. Timelines are used in so many different scenarios there is no way that one application could solve them all, with this in mind Duration was built to be extended. There are a few ways to approach customizing Duration.

### Download source bundle

Each Duration release is coupled with a Binary release as well as a source package available in the downloads section on this repository. The source package contains the application and all of its dependencies and project files for all platforms. This is the easiest way to get up and running to hack Duration. But if you want to contribute to its development consider forking the project and dependencies on Github so that you may issue Pull requests for your changes.

### Compile from Github

To compile from Github requires three things
 - A fork of the develop branch of openFrameworks
 - A fork of Duration project itself in openFrameworks/apps/
 - Forks of all dependent add-ons in openFrameworks/addons

The Duration repository has a clone_addons.sh file that can be run from the terminal:

    $ ./openFrameworks/apps/Duration/clone_addons.sh 

This will configure all necessary dependencies into openFrameworks/addons/ and the project should compile

## Version History
 - Alpha 004 released February 26th 2013
 - Alpha 003 released February 2nd 2013
 - Alpha 002 released September 23nd 2012
 - Alpha 001 released August 15th 2012

## Supported By ##

Duration is a project by [James George](http://www.jamesgeorge.org), co-developed by [YCAMInterLab](http://interlab.ycam.jp/en/) during the [Guest Research Project v.2](http://interlab.ycam.jp/en/projects/guestresearch/vol2)

<p align="center">
    <img src="http://www.jamesgeorge.org/images/ofxtimeline/github/logo_ycam.png" />
</p>

## Additional Support

![Additional Support](http://www.jamesgeorge.org/images/ofxtimeline/github/SupportedBy.png)

[Playmodes](http://www.playmodes.com/) who graciously funded and shaped the project in its early stages for [BlueBeams](https://vimeo.com/35931265).

[The Frank-Ratchye STUDIO for Creative Inquiry](http://studioforcreativeinquiry.org/) who have supported the [RGBDToolkit](http://www.rgbdtoolkit.com) project for which ofxTimeline was first created.

[Eyebeam](http://http://eyebeam.org/) who supported further development of ofxTimeline during my residency in Spring 2012.


Built with openFrameworks and ofxTimeline
