#!/bin/bash
cmake -Bbuild -GNinja && cmake --build build --config Release && mv bin/mzgl-livecode ~/bin
