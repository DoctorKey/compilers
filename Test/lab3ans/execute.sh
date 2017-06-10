#!/bin/bash

SUFFIX1=ir
SUFFIX0=out

for file in ls *cmm
do
		filename=${file%.*m}
		filename=${filename:0:3}
		echo "---------------------------start----------------------"
		echo $file	
		../lab3/Code/./parser $file "$SUFFIX0$filename.$SUFFIX1"
done
