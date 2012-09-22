# Duration

## Universal Timeline 

Duration controls change over time. With a simple one window approach, the cross platform stand alone application manages lists of tracks to compose changing data over a fixed duration.

The application sends values over OSC and can be configured through OSC messages.

![DurationInterface](http://www.duration.cc/duration_preview.png)

## Using the Duration interface

### Managing Projects

A Duration project saves a set of tracks, their values, and global configuration settings all into one package. A project is just a folder with .xml files and a special .durationproj file containing the track names and settings.

By convention Duration looks for projects in ~/Documents/Duration folder. Projects saved here will show up in the Project dropdown menu within the application. You can save projects anywhere you'd like, but you will have to remember yourself where you put them.

### Managing tracks

In Duration, tracks are the beginning and end - the alpha and omega. Everything happens inside a track. To add a track, select a type from the Add Track dropdown and it'll be appended to your composition. Name the track by clicking on the the name in the track header and typing in a new name. This name is used for all outgoing communication to identify your track, so think of it as both a name and an OSC address. It's common to put routing information right in the name, so /mytrack/position/x is a great name for a track. If you leave off the leading slash Duration adds it for you.

To remove a track, click on the delete button on the far right of the header and confirm you want it removed.

### Track Types


### Setting the duration
Every project has a fixed Duration.  ::TODO::

### In and Out points
Setting in and out points let's you focus playback on just a small part of your composition. ::TODO::

### Configuring OSC communication

::TODO::

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
</table>

## Connecting things to Duration

By itself Duration is pretty useless, as it's designed to work in close collaboration with other realtime environments.

### Receiving output

Duration sends all it's data over OSC in bundles. Whenever a value changes it well send an update (whether you are just editing the track or it's playing back). The OSC messages are formatted as follows

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
        <td>1 argument as String, Flag text (can be blank)</td>
        </tr>
    <tr>
        <td>Switch</td>
        <td>1 argument as Int32, 1 for turned on, 0 for turned off</td>
    </tr>
    <tr>
        <td>Curve</td>
        <td>1 argument as Float, Current Value, ranging between Min and Max</td>
    </tr>
    <tr>
        <td>Color</td>
        <td>3 arguments as Int32 args ranging 0-255 for Red, Green, Blue</td>
    </tr>
</table>

Whenever a new project is loaded or playback begins, Duration will always send a special information message containing information about all the tracks in the current project. The message has 4 arguments per Curves track, and 2 arguments for other tracks. The first argument is always a string with the track type, the second is the display name (which will always match the address of messages sent from that track). For Curves, the additional two arguments are floats representing the min and max values for that track.

For a project with 2 color tracks, a curves track and a flags track the info message would look like this:
     
    /duration/info Colors /colortrack_one Colors /colortrack_two Curves /accelerate -1.5 2.0 Flags /flagtrack

### Controlling Duration automatically from OSC

Duration's OSC input is always listening for you to tell it what to do next.

<table>
    <tr>
        <td style="font-weight: bold">Command</td>
        <td style="font-weight: bold">OSC Address</td>
        <td style="font-weight: bold">Arguments</td>
        <td style="font-weight: bold">Notes</td>
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
        <td>Millis (Int)</td>
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

Duration also comes packaged with a simple DurationRemote application to show and test the OSC control interface

### Recording data into tracks from OSC


## Compiling Duration from source


### Download source bundle

### Compile from Github



## Version History
 - Alpha 002 released September 22nd
 - Alpha 001 released August 15th



The core of Duration is ofxTimeline: 

Duration is the product of the [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org). 