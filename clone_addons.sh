#!/bin/bash

# Dependencies for Duration

cd ../../addons/

if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
	echo "Cloning read-only"
else
    PREFIX="git clone git@github.com:"
	echo "Cloning writable"
fi

${PREFIX}YCAMInterlab/ofxTimecode.git

${PREFIX}obviousjim/ofxTween.git

${PREFIX}obviousjim/ofxUI.git
cd ofxUI/
git checkout develop
cd ..

${PREFIX}obviousjim/ofxLocalization.git

${PREFIX}obviousjim/ofxMSATimer.git

${PREFIX}Flightphase/ofxTextInputField.git

${PREFIX}Flightphase/ofxRange.git

${PREFIX}Flightphase/ofxFTGL.git

${PREFIX}YCAMInterlab/ofxTimeline.git
cd ofxTimeline/
git checkout develop
cd ..

