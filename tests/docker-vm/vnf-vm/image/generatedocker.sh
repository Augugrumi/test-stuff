#!/bin/bash

set -e

rm -Rf astaire/

git clone https://github.com/Augugrumi/astaire.git astaire

cd astaire
git checkout 8db63f8532f4847bca20554a196eb66297a6ee63
cd -

docker build -t augugrumi/test-stuff:astaire .

rm -Rf astaire/
