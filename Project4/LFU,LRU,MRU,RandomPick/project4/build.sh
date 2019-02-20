#!/bin/sh
javac src/model/Page.java src/model/Process.java src/model/RAM.java src/scheduling/LFU.java src/scheduling/LRU.java src/scheduling/MFU.java src/scheduling/RandomPick.java src/scheduling/ScheduleHelper.java src/program/PageReplacementAlgorithm.java
cd src
jar cf project4.jar model/*.class scheduling/*.class program/*.class
mv project4.jar ../
