#!/bin/zsh
#watches drives to see that they are plugged in
function wd(){
	dir /dev | grep -oP sd.
}

