#!/bin/bash

## generate all required data
./figure7-8-data-generate.sh
./figure9-12-data-generate.sh

## draw the figures
cd results-wns3
python3 figure7a.py
python3 figure7b.py
python3 figure8a.py
python3 figure8b.py
python3 figure9a.py
python3 figure9b.py
python3 figure10a.py
python3 figure10b.py
python3 figure11.py
python3 figure12.py
python3 figure13.py
