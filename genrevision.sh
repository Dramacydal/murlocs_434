#!/usr/local/bin/bash
svn info | grep Revision | tail -c+11 > revision
svn info | grep URL | tail -c+6 >> revision
svn info | grep Date | tail -c+20 | head -c10 >> revision
echo >> revision
svn info | grep Date | tail -c+31 | head -c8 >> revision
echo >> revision