![DurationLogo](http://www.duration.cc/icon256.png)
# Duration

Universal Timeline 

Duration controls change over time. With a simple one window approach, the stand alone application manages lists of tracks to compose changing data over a fixed duration.

The application sends it's data over OSC, as well as being controllable through OSC messages.


## Using the Duration interface

### Managing tracks

### Setting the duration

### Configuring OSC communication



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